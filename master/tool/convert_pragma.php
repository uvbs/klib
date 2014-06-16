<?php


define('_key_str',   "#pragma once");

function traverse($path = '.', $project_name, $project_module) 
{
    $current_dir = opendir($path);    
    while(($file = readdir($current_dir)) !== false) {  
        $sub_dir = $path . DIRECTORY_SEPARATOR . $file;  
        if($file == '.' || $file == '..') 
        {
            continue;
        } 
        else if(is_dir($sub_dir)) 
        {
             echo 'enter sub directory: ' . $file . " \r\n";
             traverse($sub_dir, $project_name, $project_module);
        } 
        else 
        {
            $file_path = $path . '/'. $file;
            convert_file($file_path, $project_name, $project_module);
        }
    }
}

function convert_file($file, $project_name, $project_module)
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
    
    $target_str =<<<EOF
#ifndef $head_define_str
#define $head_define_str
EOF;

    $final_str = str_replace(_key_str, $target_str, $content);
    if (strlen($final_str) != strlen($content))
    {
        $final_str .= "\r\n\r\n#endif\r\n";
        
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
