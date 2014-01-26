<?php
include 'function.php';

// 用户模板配置项 
$solution_name = "template_solution";		// 解决方案名称
$solution_path = 'E:\\project\\git\\pea';		// 解决方案路径
$project_name = "pea";			// 项目名称
$main_module_name = "pea";				// 模块的名称
$common_name 	  = "common";
$klib_path = "E:\\project\\git\\klib\\master\\core";	// klib路径

$debug_mode = true;							// 是否是调试状态

// 内部配置
$solution_template_file = "template/sln_template.ini";
$solution_template_guid = "{<solution_guid>}";

$project_template_file  = "template/vcxproj_template.ini";
$project_template_name  = "{<project_name>}";
$project_template_guid  = "{<project_guid>}";

// 内部变量
$solution_guid = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";		// 解决方案的guid都是相同的
$project_guid  = create_guid();

$solution_name_path = $solution_path . "\\" . $solution_name . ".sln";
$project_name_path  = $solution_path . "\\" . $project_name . "\\" . $project_name . ".vcxproj";
$common_name_path   = $solution_path . "\\" . $common_name;
$project_bin_path 	= $solution_path . '\\' . 'bin';
$project_skins_path = $project_bin_path . '\\' . 'skins';

$module_cpp_path   = $solution_path . "\\" . $project_name . "\\" . $main_module_name . ".cpp" ;
$module_h_path   = $solution_path . "\\" . $project_name . "\\" . $main_module_name . ".h" ;
$module_dllmain_path   = $solution_path . "\\" . $project_name . "\\" . "dllmain" . ".cpp" ;
$module_stdafx_h_path   = $solution_path . "\\" . $project_name . "\\" . "stdafx" . ".h" ;
$module_stdafx_cpp_path   = $solution_path . "\\" . $project_name . "\\" . "stdafx" . ".cpp" ;

// 替换解决方案的内容
function replace_solution_contents($template_contents)
{
	global $solution_template_guid, $project_template_name, $project_template_guid;
	global $solution_guid, $project_name, $project_guid;
	
	$template_contents = str_replace($solution_template_guid, $solution_guid, $template_contents);
	$template_contents = str_replace($project_template_name,  $project_name,  $template_contents);
	$template_contents = str_replace($project_template_guid,  $project_guid,  $template_contents);
	
	return $template_contents;
}

// 替换工程的内容
function replace_project_contents($template_contents)
{
	global $solution_template_guid, $project_template_name, $project_template_guid;
	global $solution_guid, $project_name, $project_guid;
	global $main_module_name;
	
	$module_name_template = "{<Module_Name>}";
	
	$template_contents = str_replace($solution_template_guid, $solution_guid, $template_contents);
	$template_contents = str_replace($project_template_name,  $project_name,  $template_contents);
	$template_contents = str_replace($project_template_guid,  $project_guid,  $template_contents);
	$template_contents = str_replace($module_name_template,   $main_module_name,   $template_contents);
	
	return $template_contents;
}

// 如果路径存在的话那就不执行以下的操作，避免重复执行，替换了已经修改了的内容
if (file_exists($solution_path) )
{
	if (!$debug_mode)
		die("folder already exists!!! \r\n");
}

/**
 1, create a folder to hold the source.
 2, create a solution file, replace the pattern
 3, create a project file, replace the pattern
 4, create other files
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// create directory 
@mkdir($solution_path);
@mkdir($common_name_path);
@mkdir(dirname($project_name_path));
@mkdir($project_bin_path);
@mkdir($project_skins_path);
echo "create directory ... \r\n";

if (!$debug_mode)
{
	if (file_exists($solution_name_path))
		die("solution already exists!!! \r\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// 以下是处理解决方案的内容
$solution_template = file_get_contents($solution_template_file);
$solution_template = replace_solution_contents($solution_template);
file_put_contents($solution_name_path, $solution_template);

//////////////////////////////////////////////////////////////////////////////////////////////////
// 以下是处理工程的内容
$project_template = file_get_contents($project_template_file);
$project_template = replace_project_contents($project_template);
file_put_contents($project_name_path, $project_template);

//////////////////////////////////////////////////////////////////////////////////////////////////
// 以下是处理源文件内容
// 处理模块cpp
$module_contents = generate_module_cpp($main_module_name);
file_put_contents($module_cpp_path, $module_contents);

// 处理模块头文件 h
$module_contents = generate_module_hpp($main_module_name);
file_put_contents($module_h_path, $module_contents);

// 处理模块 dllmain.cpp
$module_contents = generate_dllmain($main_module_name);
file_put_contents($module_dllmain_path, $module_contents);

// 处理模块 stdafx.h
$module_contents = generate_stdafx_hpp($main_module_name);
file_put_contents($module_stdafx_h_path, $module_contents);

// 处理模块 stdafx.cpp
$module_contents = generate_stdafx_cpp($main_module_name);
file_put_contents($module_stdafx_cpp_path, $module_contents);

// 拷贝文件
@copy("template\\src\\MainFrameWnd.h", dirname($project_name_path) . "/MainFrameWnd.h");
@copy("template\\src\\MainFrameWnd.cpp", dirname($project_name_path) . "/MainFrameWnd.cpp");

@copy("template\\src\\UIWindow.h", dirname($project_name_path) . "/UIWindow.h");
@copy("template\\src\\UIWindow.cpp", dirname($project_name_path) . "/UIWindow.cpp");

@copy("template\\modules.xml", "$solution_path\\Bin\\modules.xml");
@copy("template\\skins\\mainframe.xml", $project_skins_path . '\\mainframe.xml');
@recurse_copy("template\\loader", "$solution_path\\loader");


// 链接klib模块 $klib_path  mklink /J include 
if (!file_exists("$solution_path\\include"))
{
	$cmd = "mklink /J $solution_path\\include $klib_path";
	system($cmd);
}



echo "create module complete!!! \r\n";










