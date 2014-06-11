<?php



function traverse($path = '.') 
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
             echo 'Directory ' . $file . ':<br>';
             traverse($sub_dir);
        } 
        else 
        {
            echo 'File in Directory ' . $path . ': ' . $file . "\r\n";
            
            $file_path = $path . '/'. $file;
            convert_file($file_path);
        }
    }
}

define('key_str',   "#pragma once");
define("lib_namespace", "_klib");

function convert_file($file)
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
    $head_define_str = lib_namespace . '_' . $head_define_str ; //. "_h";
    
    $target_str =<<<EOF
#ifndef $head_define_str
#define $head_define_str
EOF;

    $final_str = str_replace(key_str, $target_str, $content);
    if (strlen($final_str) != strlen($content))
    {
        $final_str .= "\r\n\r\n#endif\r\n";
    }
    
    file_put_contents($file, $final_str);
}

$argc = $_SERVER['argc'];
$argv = $_SERVER['argv'];
//traverse();
if ($argc <= 1)
{
    die("php convert_pragma.php file_path");
}

$file_to_convert = $argv[1];
traverse($file_to_convert);

?>
