<?php
// $Id$

function textbook_companion_search_form()
{
  $form['#redirect'] = FALSE;
  $form['search'] = array(
      '#type' => 'textfield',
      '#title' => t('Search'),
      '#size' => 48,
  );

  $form['search_by_title'] = array(
      '#type' => 'checkbox',
      '#default_value' => TRUE,
      '#title' => t('Search by Title of the Book'),
  );

  $form['search_by_author'] = array(
      '#type' => 'checkbox',
      '#default_value' => TRUE,
      '#title' => t('Search by Author of the Book'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), ''),
  );

  if ($_POST)
  {
    $output = '';
    $search_rows = array();
    $search_query = '';
    if ($_POST['search_by_title'] && $_POST['search_by_author'])
      $search_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 AND (book LIKE '%%%s%%' OR author LIKE '%%%s%%')", $_POST['search'], $_POST['search']);
    else if ($_POST['search_by_title'])
      $search_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 AND book LIKE '%%%s%%'", $_POST['search']);
    else if ($_POST['search_by_author'])
      $search_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 AND author LIKE '%%%s%%'", $_POST['search']);
    else
      drupal_set_message('Please select whether to search by Title and/or Author of the Book.', 'error');
    while ($search_data = db_fetch_object($search_q))
    {
      $search_rows[] = array(l($search_data->book, 'textbook_run/' . $search_data->id), $search_data->author); 
    }
    if ($search_rows)
    {
      $search_header = array('Title of the Book', 'Author Name');
      $output .= theme_table($search_header, $search_rows);
      $form['search_results'] = array(
          '#type' => 'item',
          '#title' => t('Search results for "') . $_POST['search'] . '"',
          '#value' => $output,
      );
    } else {
      $form['search_results'] = array(
          '#type' => 'item',
          '#title' => t('Search results for "') . $_POST['search'] . '"',
          '#value' => 'No results found',
      );
    }
  }
  return $form;
}

/******************************************************************************/
/**************************** BROWSE BY FORMS *********************************/
/******************************************************************************/

function textbook_companion_browse_book()
{
  $return_html = _browse_list('book');
  $return_html .= '<br /><br />';
  $query_character = arg(2);
  if (!$query_character) {
    /* all books */
    $return_html .= "Please select the starting character of the title of the book";
    return $return_html;
  }
  $book_rows = array();
  $book_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE book like '%s%%' AND approval_status = 1", $query_character);
  while ($book_data = db_fetch_object($book_q))
  {
    $book_rows[] = array(l($book_data->book, 'textbook_run/' . $book_data->id), $book_data->author);
  }
  if (!$book_rows)
  {
    $return_html .= "Sorry no books are available with that title";
  } else {
      $book_header = array('Title of the Book', 'Author Name');
      $return_html .= theme_table($book_header, $book_rows);
  }
  return $return_html;
}

function textbook_companion_browse_author()
{
  $return_html = _browse_list('author');
  $return_html .= '<br /><br />';
  $query_character = arg(2);
  if (!$query_character) {
    /* all books */
    $return_html .= "Please select the starting character of the author's name";
    return $return_html;
  }
  $book_rows = array();
  $book_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE author like '%s%%' AND approval_status = 1", $query_character);
  while ($book_data = db_fetch_object($book_q))
  {
    $book_rows[] = array(l($book_data->book, 'textbook_run/' . $book_data->id), $book_data->author);
  }
  if (!$book_rows)
  {
    $return_html .= "Sorry no books are available with that author's name";
  } else {
      $book_header = array('Title of the Book', 'Author Name');
      $return_html .= theme_table($book_header, $book_rows);
  }
  return $return_html;
}

function _browse_list($type)
{
  $return_html = '';
  $char_list = array('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z');
  foreach ($char_list as $char_name)
  {
    $return_html .= l($char_name, 'textbook_search/' . $type . '/' . $char_name);
    if ($char_name != 'Z')
      $return_html .= ' | ';
  }
  return $return_html;
}
