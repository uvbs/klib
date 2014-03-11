<?php

$argc = $_SERVER['argc'];
$argv = $_SERVER['argv'];

if ($argc <= 2) {
	die("need input file");
}
$src_file = $argv[1];
$target_file = $argv[2];

$file_content = file_get_contents($src_file);
//echo $file_content;

//整个协议的注释 
class protocol_enum {
	public $name;
	public $comment;
	public $items;
}

class protocol_member {
	public $name;
	public $type;
	public $value;
	public $initval; //初始值
	public $comment;
}

class protocol_item {
	public $content;
	public $name;
	public $comment;
	public $members;
}

function Parse($file_content) {
	//parse protocol enum
	$enum_patten = "/\/?\/?([^\n]*)?\s*typedef\s+enum\s*([^ {]+)\s*{([^}]+)\s*}[^;]+;/i";
	$matches = array();		//save matches contents
	$enum_match_count = preg_match_all($enum_patten, $file_content, $matches);
	
	//
	//$items_pattern = "/[\/\/]?([^\n]*)?\s*([^,]*),\s*\/\*([^*]*)\*\//i";
	$items_pattern = "/[\/\/]?([^\n]*)?\s+([^,\/]*),\s*\/\*([^*]*)\*\//i";
	function trim_value(&$value) {
		$value = trim($value);
	}
	
	$whole_protocol_enum = array();
	for ($i=0; $i<$enum_match_count; ++$i) {
		$protocol_ = new protocol_enum();
		$protocol_->comment = $matches[1][$i];
		$protocol_->name = $matches[2][$i];;//$name;
		$protocol_->items = array();
		
		$protocol_content = $matches[3][$i];
		//echo $protocol_content;
	
		$item_matches = array();
		$item_count = preg_match_all($items_pattern, $protocol_content, $item_matches);
		//echo $item_count;
		//print_r($item_matches);
		//exit;
		for ($j=0; $j<$item_count; ++$j) {
			$item_ = new protocol_item;
			$item_->comment = trim($item_matches[1][$j]);
			$item_->name = trim($item_matches[2][$j]);
			$item_->content = trim($item_matches[3][$j]);
			
			//echo $item_->name."---\r\n";
			
			
			$arr = explode ('=', $item_->name);
			if (count($arr) > 1) {  
				$item_->name = trim($arr[0]);
			}
			$arr_member = explode("\r\n", $item_->content);
	
			array_walk($arr_member, 'trim_value');
			$arr_member = array_filter($arr_member);
			//print_r($arr_member);
	
			$item_->members = array();
			foreach ($arr_member as $tmp) {
				$arr = explode(' : ', $tmp);
				if (count($arr) < 2) {
					$arr = explode('	:	', $tmp);
				}
				$type = trim($arr[0]);
				
				$arr = explode(';', trim($arr[1]));
				$value = trim($arr[0]);
				$comment = isset($arr[1]) ? trim($arr[1]) : '';
				$initval = '';
				
				//判断是否有初始值
				//$intval
				$arr = explode('=', $value);
				if (count($arr) == 2) {
					$value = trim($arr[0]);
					$initval = trim($arr[1]);
				}
				
				//去掉双斜框
				$pos = strpos($comment, '//');
				if ($pos !== false) {
					$comment = substr($comment, 2);
				}
					
				$member_ = new protocol_member;
				$member_->type = $type;
				$member_->value = $value;
				$member_->initval = $initval;
				$member_->comment = $comment;
				$item_->members[] = $member_;
			}
	
			$protocol_->items[] = $item_;
		}
	
		$whole_protocol_enum[] = $protocol_;
	}
	
	/*
	echo "\r\n------------------------------------------------------------------------------\r\n";
	
	$con = print_r($whole_protocol_enum, true);
	file_put_contents('x.cpp', $con);
	//print_r($enum_match_count);
	
	echo "\r\n------------------------------------------------------------------------------\r\n";
	
	print_r($matches);
	
	return $whole_protocol_enum;
	*/
	
	return $whole_protocol_enum;
}



/////write to file

$protocol_ = new protocol_enum;
$item_ = new protocol_item;
$member_ = new protocol_member;

$result = Parse($file_content);
//print_r($result);

$generate_content =<<<EOF
#ifndef _PROTOCOL_DEF_H_
#define _PROTOCOL_DEF_H_

#include "stdio.h"
#include "protocol.h"
#include <net/proto/net_archive.h>
#include <net/proto/command_header.h>

using namespace klib::net;

// #pragma pack(1)

EOF;

foreach ($result as $protocol_) {
	
	$generate_content .= "\r\n/////////////////////////////////////////////////////////////////////////\r\n//开始定义协议: {$protocol_->name} ";
	
	foreach ($protocol_->items as $item_) {
		////////////////////////////////////////////////////////////////////////////////////
		$st_name = 'PT_'.$item_->name;
		
		$generate_head =<<<EOF

//{$item_->comment}
struct {$st_name} {
EOF;

		$generate_foot = "};\r\n\r\n";
		$generate_init = '';
		$generate_members = array();
		$generate_serilisze = array();
		$generate_deserilise = array();

		$initval_arry = array();

		$members = array();
		if (is_array($item_->members)) {
			$members = $item_->members;
		}

		foreach ($members as $member_) {
			$comments = empty($member_->comment) ? '' : " \t// ".$member_->comment;
			
			$generate_members[] = "{$member_->type}  {$member_->value};  {$comments}";
			$generate_serilisze[] = "ar << pt.{$member_->value}; ";
			$generate_deserilise[] = "ar >> pt.{$member_->value}; ";
			
			if (!empty($member_->initval)) {
				$initval_arry[] = "{$member_->value}($member_->initval)";
			}
		}
		$generate_members = implode("\r\n	", $generate_members);
		$generate_serilisze = implode("\r\n		", $generate_serilisze);
		$generate_deserilise = implode("\r\n		", $generate_deserilise);
		
		if (count($initval_arry) > 0) {
			$generate_init = implode(',', $initval_arry);
			$generate_init = ': '.$generate_init;
		}
		
		$generate_content .=<<<EOF
{$generate_head}
	$st_name () {$generate_init} {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = $item_->name;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, {$st_name}& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		{$generate_serilisze}
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, {$st_name}& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		{$generate_deserilise}
		return ar;
	}
	
	
	{$generate_members}
{$generate_foot}
EOF;
		////////////////////////////////////////////////////////////////////////////////////
	}

}

$generate_content .=<<<EOF

// #pragma pack()

#endif

EOF;


file_put_contents($target_file, $generate_content );

?>