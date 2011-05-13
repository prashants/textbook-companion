<?php

function textbook_companion_download_book()
{
  $preference_id = arg(2);
  _latex_generate_files($preference_id);
}

function _latex_generate_files($preference_id)
{
  $book_filedata = "";
  $contributor_filedata = "";
  $latex_filedata = "";

  $eol = "\n";

  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $preference_id);
  $preference_data = db_fetch_object($preference_q);
  if (!$preference_data)
  {
    drupal_set_message('Invalid book specified.', 'error');
    drupal_goto('');
  }
  if ($preference_data->approval_status == 0)
  {
    drupal_set_message('Book proposal is still in pending review.', 'error');
    drupal_goto('');
  }
  $book_filedata = $preference_data->book . ':' . $preference_data->author . ':' . $preference_data->isbn . ':' . $preference_data->publisher . ':' . $preference_data->edition . ':' . $preference_data->year . $eol;

  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message('Could not fetch contributors information for the book specified.', 'error');
  }
  $contributor_filedata .= $proposal_data->full_name . ':' . $proposal_data->course . ':' . $proposal_data->branch . ':' . $proposal_data->university . ':' . $proposal_data->faculty . ':' . $proposal_data->reviewer . $eol;

  $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d ORDER BY number DESC", $preference_data->id);
  while ($chapter_data = db_fetch_object($chapter_q))
  {
    $example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d AND approval_status = 1 ORDER BY number DESC", $chapter_data->id);
    while ($example_data = db_fetch_object($example_q))
    {
      $example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_data->id);
      while ($example_files_data = db_fetch_object($example_files_q))
      {
        $latex_filedata .= $chapter_data->number . ':';
        $latex_filedata .= $chapter_data->name . ':';
        $latex_filedata .= $example_data->number . ':';
        $latex_filedata .= $example_data->caption . ':';
        $latex_filedata .= $example_files_data->filename . ':';
        $latex_filedata .= $example_files_data->filepath . ':';
        $latex_filedata .= $example_files_data->filetype . ':';
        $latex_filedata .= ':';
        $latex_filedata .= $example_files_data->id;
        $latex_filedata .= $eol;
      }
      $dependency_files_q = db_query("SELECT * FROM {textbook_companion_example_dependency} WHERE example_id = %d", $example_data->id);
      while ($dependency_files_data = db_fetch_object($dependency_files_q))
      {
        $dependency_q = db_query("SELECT * FROM {textbook_companion_dependency_files} WHERE id = %d LIMIT 1", $dependency_files_data->dependency_id);
        if ($dependency_data = db_fetch_object($dependency_q))
        {
          $latex_filedata .= $chapter_data->number . ':';
          $latex_filedata .= $chapter_data->name . ':';
          $latex_filedata .= $example_data->number . ':';
          $latex_filedata .= $example_data->caption . ':';
          $latex_filedata .= $dependency_data->filename . ':';
          $latex_filedata .= $dependency_data->filepath . ':';
          $latex_filedata .= 'D' . ':';
          $latex_filedata .= $dependency_data->caption . ':';
          $latex_filedata .= $dependency_data->id;
          $latex_filedata .= $eol;
        }
      }
    }
  }

  /**************************** WRITE TO FILES ********************************/
  $root_path = textbook_companion_path();
  $download_filename = $preference_data->book . "_" . $preference_data->author;
  $book_filename = "tmp_" . $preference_data->id . "_book.txt";
  $contributor_filename = "tmp_" . $preference_data->id . "_contributor.txt";
  $latex_filename = "tmp_" . $preference_data->id . "_data.txt";
  $pdf_filename = "tmp_" . $preference_data->id . ".pdf";

  $fb = fopen($root_path . $book_filename, 'w');
  fwrite($fb, $book_filedata);
  fclose($fb);

  $fc = fopen($root_path . $contributor_filename, 'w');
  fwrite($fc, $contributor_filedata);
  fclose($fc);

  $fl = fopen($root_path . $latex_filename, 'w');
  fwrite($fl, $latex_filedata);
  fclose($fl);

  //_latex_run_script($book_filename, $contributor_filename, $latex_filename, $pdf_filename);

  /*********************** DELETING TEMPORARY FILES ***************************/
  //unlink($root_path . $book_filename);
  //unlink($root_path . $contributor_filename);
  //unlink($root_path . $latex_filename);
  //unlink($root_path . $pdf_filename);
}

function _latex_copy_script_file()
{
}

function _latex_run_script($book_filename, $contributor_filename, $latex_filename, $pdf_filename)
{
  $ret;
  $command_params = $book_filename . " " . $contributor_filename . " " . $latex_filename;
  $script_output = system('ls', $ret);
  if ($ret == 0)
    return TRUE;
  else
    return FALSE;
}

