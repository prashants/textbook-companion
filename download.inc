<?php
// $Id$

function textbook_companion_download_example()
{
  $example_id = arg(2);
  $root_path = $_SERVER['DOCUMENT_ROOT'] . base_path();

  /* get example data */
  $example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d", $example_id);
  $example_data = db_fetch_object($example_q);
  $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
  $chapter_data = db_fetch_object($chapter_q);
  $example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);

  /* zip filename */
  $zip_filename = $root_path . 'uploads/zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($zip_filename, ZipArchive::CREATE);

  while ($example_files_row = db_fetch_object($example_files_q))
  {
    if ($example_files_row->filetype == 'D')
      $zip->addFile($root_path . $example_files_row->filepath, 'DEPENDENCIES/' . $example_files_row->filename);
    else
      $zip->addFile($root_path . $example_files_row->filepath, $example_files_row->filename);
  }
  $zip->close();

  /* download zip file */
  header('Content-Type: application/zip');
  header('Content-disposition: attachment; filename="example.zip"');
  header('Content-Length: ' . filesize($zip_filename));
  readfile($zip_filename);
  unlink($zip_filename);
}

function textbook_companion_download_chapter()
{
  
}

function textbook_companion_download_book()
{
  
}

