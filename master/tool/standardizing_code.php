<?php


define('_key_str',          "#pragma once");
define('_paragma_pattern',  '#pragma\\s+once\\s*\\r');


// 遍历文件夹，并回调
function traverse($path = '.', $callback) 
{
    $current_dir = opendir($path);    
    while(($file = readdir($current_dir)) !== false) 
    {  
        $sub_dir = $path . DIRECTORY_SEPARATOR . $file;  
        if($file == '.' || $file == '..') 
        {
            continue;
        } 
        else if(is_dir($sub_dir)) 
        {
             echo 'enter sub directory: ' . $file . " \r\n";
             traverse($sub_dir, $callback);
        }
        else 
        {
            $file_path = $path . '/'. $file;
            $callback($file_path);
        }
    }
    closedir($current_dir);
}

interface icode_standardizing
{
    public function deal_file($filename);
}

class std_comment implements icode_standardizing
{
    private var $author;                // author  dk
    private var $version;               // version 1.1.1.1
    private var $comment_format;        // 注释的格式
    private var $comment_flag;          // 用于检测是否有注释的,如果有注释，则不在添加注释
    
    private var $header_ext;            // 头文件的扩展名
    private var $cpp_ext;               // cpp的扩展名
    
    function __construct() 
    {
        $this->header_ext = array('.h', '.hpp');
        $this->cpp_ext    = array('.cpp');
    }
    
    ////////////////////////////////////////////////////////////////////////
    private function add_header_comment($file_name)
    {
        
    }
    
    private function add_cpp_comment($file_name)
    {
        
    }
    
}

class std_header implements icode_standardizing
{
    var $si_define_upper;           // 是否转换成大写的
    var $exclude_files;             // 排除的文件
    

    function __construct() 
    {
        $this->exclude_files = array(
            'stdafx.hpp', 'stdafx.h', "stdafx.cpp", 'targetver.h'
        );
        
    }
    
    
    ////////////////////////////////////////////////////////////////////////
    private function modify_header($file_name)
    {
        
    }
    
    private function modify_param_header($file_name)
    {
        
    }
}


// 转换头文件中的pragma为define
function convert_pragma_2_define($file, $project_name, $project_module)
{
    $base_name = basename($file);
    
    $content = file_get_contents($file);
    if (!$content)
    {
        return;
    }
    
    $path_parts = pathinfo($file);
    if ($path_parts['extension'] != "h")
    {
        return;
    }
    
    $head_define_str = $base_name;
    $head_define_str = str_replace(".", "_", $head_define_str);
    
    if (!empty($project_module))
    {
        $head_define_str = '_' . $project_name . '_' . $project_module. '_' . $head_define_str ; //. "_h";
    }
    else
    {
        $head_define_str = '_' . $project_name . '_' . $head_define_str ; //. "_h";
    }
    if (_upper_define)
    {
        $head_define_str = strtoupper($head_define_str);
    }
    
    $target_str =<<<EOF
#ifndef $head_define_str
#define $head_define_str
EOF;

    $final_str = str_replace(_key_str, $target_str, $content);
    if (strlen($final_str) != strlen($content))
    {
        $final_str .=<<<EOF

#endif  // $head_define_str

EOF;
        
        file_put_contents($file, $final_str);
        
        echo "converted file: $file \r\n";
    }
}

$argc = $_SERVER['argc'];
$argv = $_SERVER['argv'];

//traverse();
if ($argc < 2)
{
    die("php convert_pragma.php file_path");
}
$dir_to_convert = $argv[1];


$project_name = "klib";
$project_module = "";

echo "project: $project_name, module: $project_module \r\n";
traverse($dir_to_convert, $project_name, $project_module);


?>
