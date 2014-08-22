<?php


///

$file_to_change = array
(
	'./common/common_info.h',
	'./setup/common.nsh',			// 打包的公共头文件
	'./setup/sign-command.bat',		// 签名的批处理命令
	//'./client/client.rc'
);

// 前一个版本号
$prev_version_1 = '1.2.1.2';
$prev_version_2 = '1,2,1,2';

// 后一个版本号
$next_version_1 = '1.2.1.3';
$next_version_2 = '1,2,1,3';

foreach ($file_to_change as $file)
{
	$content = file_get_contents($file);
	if ($content)
	{
		echo "$file \r\n";
		//echo $content;
		
		$content = str_replace($prev_version_1, $next_version_1, $content);
		$content = str_replace($prev_version_2, $next_version_2, $content);
		
		file_put_contents($file, $content);
	}
}

// UTF-16LE编码的要特殊处理一下
$file = './client/client.rc';
$content = file_get_contents($file);
if ($content)
{
	$content = iconv('UTF-16LE', 'UTF-8', $content);
	
	$content = str_replace($prev_version_1, $next_version_1, $content);
	$content = str_replace($prev_version_2, $next_version_2, $content);
	
	$content = iconv('UTF-8', 'UTF-16LE', $content);
	file_put_contents($file, $content);
}


?>