<?php
// $Id$

function list_chapters()
{
  global $user;

  /* get approved book details */
  $book_q = db_query("SELECT pro.id as pro_id, pre.id as pre_id, pre.book as pre_book, pro.full_name as pro_full_name, pre.book as pre_book FROM {textbook_companion_proposal} pro JOIN {textbook_companion_preference} pre ON pro.id = pre.proposal_id WHERE pro.proposal_status = 1 AND pro.uid = %d", $user->uid);
  $book_data = db_fetch_object($book_q);
  if (!$book_data)
  {
    drupal_set_message("Your book proposal has not been approved yet. You will receive a email from us when your proposal is approved.", 'error');
    drupal_goto('');
    return;
  }

  $return_html = '';
  $return_html .= '<strong>Book Name : </strong>' . $book_data->pre_book . '<br />';

  /* get chapter list */
  $chapter_rows = array();
  $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d", $book_data->pre_id);
  while ($chapter_data = db_fetch_object($chapter_q))
  {
    /* get example list */
    $example_q = db_query("SELECT count(*) as example_count FROM {textbook_companion_example} WHERE chapter_id = %d", $chapter_data->id);
    $example_data = db_fetch_object($example_q);
    $chapter_rows[] = array($chapter_data->number, $chapter_data->name, $example_data->example_count, l('View', 'textbook_companion/list_examples/' . $chapter_data->id));
  }

  /* check if there are any chapters */                                   
  if (!$chapter_rows)
  {
    drupal_set_message(t('No uploads found.'), 'status');
    return $return_html;
  }

  $chapter_header = array('Chapter No.', 'Chapter Title', 'No. Of Examples', 'Actions');
  $return_html .= theme_table($chapter_header, $chapter_rows);
  return $return_html; 
}

function list_examples()
{
  $chapter_id = arg(2);

  /* get example list */
  $example_rows = array();
  $example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d", $chapter_id);
  while ($example_data = db_fetch_object($example_q))
  {
    /* approval status */
    $approval_status = '';
    switch ($example_data->approval_status)
    {
    case 0: $approval_status = 'Pending'; break;
    case 1: $approval_status = 'Approved'; break;
    case 1: $approval_status = 'Rejected'; break;
    }

    /* example files */
    $example_files = '';
    $example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d ORDER BY filetype", $example_data->id);
    while ($example_files_data = db_fetch_object($example_files_q))
    {
      $example_files .= $example_files_data->filename . ' (' . $example_files_data->filetype . ')<br />';
    }
    $example_rows[] = array($example_data->number, $example_data->caption, $approval_status, $example_files);
  }
  $example_header = array('Example No.', 'Caption', 'Status', 'Files');
  $return_html .= theme_table($example_header, $example_rows);
  return $return_html;
}                                                       
