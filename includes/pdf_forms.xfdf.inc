<?php

/**
* Generates the XFDF file format data for the PDF.
*/
function pdf_forms_generate_xfdf($file, $map, $encode = 'UTF-8') {
  $data = '<?xml version="1.0" encoding="' . $encode . '"?>' . "\n" .
    '<xfdf xmlns="http://ns.adobe.com/xfdf/" xml:space="preserve">' . "\n" .
    '<fields>' . "\n";
  foreach ($map as $name => $value) {
    $name = str_replace('-', ' ', $name);
    $data .= '<field name="' . htmlspecialchars($name) . '"><value>' . htmlspecialchars($value) . '</value></field>' . "\n";
  }
  $data .= '</fields>' . "\n" .
    '<ids original="' . md5($file) . '" modified="' . REQUEST_TIME . '" />' . "\n" .
    '<f href="' . $file . '" />' . "\n" .
    '</xfdf>' . "\n";


  return $data;
}
