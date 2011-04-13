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
    '#value' => l(t('Cancel'), 'textbook_companion/code'),
  );

  if ($_POST)
  {
    $output = '';
    $search_rows = array();
    $search_query = '';
    if ($_POST['search_by_title'] && $_POST['search_by_author'])
      $search_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE book LIKE '%%%s%%' OR author LIKE '%%%s%%'", $_POST['search'], $_POST['search']);
    else if ($_POST['search_by_title'])
      $search_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE book LIKE '%%%s%%'", $_POST['search']);
    else if ($_POST['search_by_author'])
      $search_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE author LIKE '%%%s%%'", $_POST['search']);
    else
      drupal_set_message('Please select whether to search by Title and/or Author of the Book.', 'error');
    while ($search_data = db_fetch_object($search_q))
    {
      $search_rows[] = array(l($search_data->book, ''), $search_data->author); 
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

