<?php
// $Id$

function textbook_companion_download_example()
{
  $example_id = arg(2);
  $dest_path = $_SERVER['DOCUMENT_ROOT'] . base_path() . 'uploads/';

  /* get example data */
  $example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d", $example_id);
  $example_data = db_fetch_object($example_q);
  $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
  $chapter_data = db_fetch_object($chapter_q);
  //$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $chapter_data->preference_id);
  //$preference_data = db_fetch_object($preference_q);

  /* getting example path */
  $dest_path .= $chapter_data->preference_id . '/';
  $dest_path .= 'CH' . $chapter_data->number . '/';
  $dest_path .= 'EX' . $example_data->number . '/';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open('uploads/tempfile.zip', ZipArchive::CREATE);
  if (false !== ($dir = opendir($dest_path)))
  {
    while (false !== ($file = readdir($dir)))
    {
      if ($file != '.' && $file != '..')
      {
        $zip->addFile($dest_path . $file, $file);
      }
    }
  } else {
     die('Can\'t read dir');
  }

  /* zip file name */
  $zip_filename = $_SERVER['DOCUMENT_ROOT'] . base_path() . 'uploads/tempfile.zip';

  /* download zip file */
  header('Content-Type: application/zip');
  header('Content-disposition: attachment; filename="example.zip"');
  header('Content-Length: ' . filesize($zip_filename));
  readfile($zip_filename);
  $zip->close();

/* $zip = new ZipArchive;
$zip->open('uploads/myzip.zip', ZipArchive::CREATE);
foreach (glob($dest_path . '*') as $file) {
  echo $file;
    $zip->addFile($file, $file);
}
$zip->close();
var_dump($zip);

$zipfilename = '/var/www/textbook/uploads/myzip.zip';
var_dump(filesize($zipfilename)); die();
header('Content-Type: application/zip');
header('Content-disposition: attachment; filename=myzip.zip');
header('Content-Length: ' . filesize($zipfilename));
readfile($zipfilename); */
}

