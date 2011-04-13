<?php

function textbook_companion_search_page()
{
  $output .= drupal_get_form('textbook_companion_search_form');

  $search_rows = array();
  $search_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE book LIKE '%tip%'");
  while ($search_data = db_fetch_object($search_q))
  {
    $search_rows[] = array($search_data->book, $search_data->author); 
  }
  $search_header = array('Title of the Book', 'Author Name');
  $output .= theme_table($search_header, $search_rows);
  return $output;
}

function textbook_companion_search_form()
{
  $form['search'] = array(
      '#type' => 'textfield',
      '#title' => t('Search'),
      '#size' => 48,
  );

  $form['search_by_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Search by title of the book'),
  );

  $form['search_by_author'] = array(
      '#type' => 'checkbox',
      '#title' => t('Search by author of the book'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'textbook_companion/code'),
  );
  return $form;
}

function textbook_companion_search_form_submit($form, &$form_state)
{
  $output = drupal_get_form('textbook_companion_search_form');
  $search_rows = array();
  $search_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE book LIKE '%" . $form_state['values']['search'] . "%'");
  while ($search_data = db_fetch_object($search_q))
  {
    $search_rows[] = array($search_data->book, $search_data->author); 
  }
  $search_header = array('Title of the Book', 'Author Name');
  $output .= theme_table($search_header, $search_rows);
  return $output;
}
