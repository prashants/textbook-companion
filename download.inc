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

  /* temporary zip filename */
  $temp_zip_filename = 'uploads/zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($temp_zip_filename, ZipArchive::CREATE);
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
  $zip->close();

  /* zip file name */
  $zip_filename = $_SERVER['DOCUMENT_ROOT'] . base_path() . $temp_zip_filename;

  /* download zip file */
  header('Content-Type: application/zip');
  header('Content-disposition: attachment; filename="example.zip"');
  header('Content-Length: ' . filesize($zip_filename));
  readfile($zip_filename);
  unlink($zip_filename);
}

