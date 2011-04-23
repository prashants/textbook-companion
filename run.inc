<?php
// $Id$

function textbook_companion_run_form($form_state)
{
  $form['#redirect'] = FALSE;

  ahah_helper_register($form, $form_state);

  /* default value for ahah fields */
  if (!isset($form_state['storage']['run']['book']))
  {
    /* get the book id from url */
    $url_book_id = (int)arg(1);
    if ($url_book_id)
    {
      /* add javascript for book selected */
      $chapter_name_js = " $(document).ready(function() {
        $('#edit-run-book').val(" . $url_book_id . ");
        $('#edit-run-book').change();
      });";
      drupal_add_js($chapter_name_js, 'inline', 'footer');
    } else {
      $book_default_value = 0;
    }
  } else {
    $book_default_value =  $form_state['storage']['run']['book'];
  }

  if (!isset($form_state['storage']['run']['chapter']))
  {
    $chapter_default_value = 0;
  } else {
    if ($form_state['values']['run']['book_hidden'] !=  $form_state['values']['run']['book'])
      $chapter_default_value = 0;
    else
      $chapter_default_value = $form_state['storage']['run']['chapter'];
  }

  if (!isset($form_state['storage']['run']['example']))
  {
    $example_default_value = 0;
  } else {
    if ($form_state['values']['run']['book_hidden'] !=  $form_state['values']['run']['book'])
      $example_default_value = 0;
    else if ($form_state['values']['run']['chapter_hidden'] !=  $form_state['values']['run']['chapter'])
      $example_default_value = 0;
    else
      $example_default_value = $form_state['storage']['run']['example'];
  }

  $form['run'] = array(
    '#type' => 'fieldset',
    '#title' => t('Run Book Example'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="run-wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  $form['run']['book'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Book'),
      '#options' => _list_of_books(),
      '#default_value' => $book_default_value,
      '#tree' => TRUE,
      '#ahah' => array(
        'event'   => 'change',
        'effect' => 'none',
        'path'    => ahah_helper_path(array('run')),
        'wrapper' => 'run-wrapper',
        'progress' => array(
          'type' => 'throbber',
          'message' => t(''),
        ),
      ),
  );

  /* hidden form elements */
  $form['run']['book_hidden'] = array(
    '#type'  => 'hidden',
    '#value' =>  $form_state['values']['run']['book'],
  );

  /* hidden form elements */
  $form['run']['chapter_hidden'] = array(
    '#type'  => 'hidden',
    '#value' =>  $form_state['values']['run']['chapter'],
  );

  if ($book_default_value > 0)
  {
    $form['run']['download_book'] = array(
      '#type' => 'item',
      '#value' => l('Download', 'download/book/' . $book_default_value) . ' ' . t('(Download all the examples of the entire book)'),
    );
    $form['run']['contributor_name'] = array(
      '#type' => 'item',
      '#value' => '<strong>Contributor name : </strong>' . _contributor_name($book_default_value),
    );
    $form['run']['chapter'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Chapter'),
      '#options' => _list_of_chapters($book_default_value),
      '#default_value' => $chapter_default_value,
      '#tree' => TRUE,
      '#ahah' => array(
        'event'   => 'change',
        'effect' => 'none',
        'path'    => ahah_helper_path(array('run')),
        'wrapper' => 'run-wrapper',
        'progress' => array(
          'type' => 'throbber',
          'message' => t(''),
        ),
      ),
    );
    if ($chapter_default_value > 0)
    {
      $form['run']['download_chapter'] = array(
        '#type' => 'item',
        '#value' => l('Download', 'download/chapter/' . $chapter_default_value) . ' ' . t('(Download all the examples of the entire chapter)'),
      );
      $form['run']['example'] = array(
        '#type' => 'select',
        '#title' => t('Example No.'),
        '#options' => _list_of_examples($chapter_default_value),
        '#default_value' => $example_default_value,
        '#tree' => TRUE,
        '#ahah' => array(
          'event'   => 'change',
          'effect' => 'none',
          'path'    => ahah_helper_path(array('run')),
          'wrapper' => 'run-wrapper',
          'progress' => array(
            'type' => 'throbber',
            'message' => t(''),
          ),
        ),
      );
    }
  }

  /* hidden form elements */
  $form['run']['update_book'] = array(
    '#type'  => 'submit',
    '#value' => t('Update'),
    '#submit' => array('ahah_helper_generic_submit'),
    '#attributes' => array('class' => 'no-js'),
  );

  $form['run']['update_chapter'] = array(
    '#type'  => 'submit',
    '#value' => t('Update'),
    '#submit' => array('ahah_helper_generic_submit'),
    '#attributes' => array('class' => 'no-js'),
  );

  /************ START OF $_POST **************/
  if ($_POST)
  {
    if (($book_default_value > 0) && ($chapter_default_value > 0) && ($example_default_value > 0))
    {
      $example_list_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $form_state['values']['run']['example']);
      if ($example_list_q)
      {
        $example_files_rows = array();
        while ($example_list_data = db_fetch_object($example_list_q))
        {
          $example_file_type = '';
          switch ($example_list_data->filetype)
          {
          case 'S' : $example_file_type = 'Source or Main file'; break;
          case 'R' : $example_file_type = 'Result file'; break;
          case 'X' : $example_file_type = 'xcos file'; break;
          default  : $example_file_type = 'Unknown'; break;
          }
          $example_files_rows[] = array(l($example_list_data->filename, 'download/file/' . $example_list_data->id), $example_file_type);
        }

        /* dependency files */
        $dependency_list_q = db_query("SELECT dependency.id as dependency_id, dependency.filename as dependency_filename, dependency.caption as dependency_caption
          FROM {textbook_companion_example_dependency} example_dependency LEFT JOIN {textbook_companion_dependency_files} dependency
          ON example_dependency.dependency_id = dependency.id
          WHERE example_dependency.example_id = %d", $form_state['values']['run']['example']);
          while ($dependency_list_data = db_fetch_object($dependency_list_q))
          {
            $example_file_type = 'Dependency file';
            $temp_caption = '';
            if ($dependency_list_data->dependency_caption)
              $temp_caption = ' (' . $dependency_list_data->dependency_caption . ')';
            $example_files_rows[] = array(l($dependency_list_data->dependency_filename, 'download/dependency/' . $dependency_list_data->dependency_id) . $temp_caption, $example_file_type);
          }

        /* creating list of files table */
        $example_files_header = array('Filename', 'Type');
        $example_files = theme_table($example_files_header, $example_files_rows); 
      }
      $form['run']['download_example'] = array(
        '#type' => 'item',
        '#value' => l('Download Example', 'download/example/' . $example_default_value),
      );
      $form['run']['example_files'] = array(
        '#type'  => 'item',
        '#title' => 'List of example files',
        '#value' => $example_files,
      );
      $form['run']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Run')
      );
    }
  }
  /************ END OF $_POST **************/

  return $form;
}

function textbook_companion_run_form_submit($form, &$form_state)
{

}

function _list_of_books()
{
  $book_titles = array('0' => 'Please select...');
  $book_titles_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 OR approval_status = 3 ORDER BY book ASC");
  while ($book_titles_data = db_fetch_object($book_titles_q))
  {
    $book_titles[$book_titles_data->id] = $book_titles_data->book . ' (Written by ' . $book_titles_data->author . ')';
  }
  return $book_titles;
}

function _list_of_chapters($preference_id = 0)
{
  $book_chapters = array('0' => 'Please select...');
  $book_chapters_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d ORDER BY number ASC", $preference_id);
  while ($book_chapters_data = db_fetch_object($book_chapters_q))
  {
    $book_chapters[$book_chapters_data->id] = $book_chapters_data->number . '. ' . $book_chapters_data->name;
  }
  return $book_chapters;
}

function _list_of_examples($chapter_id = 0)
{
  $book_examples = array('0' => 'Please select...');
  $book_examples_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d AND approval_status = 1 ORDER BY number ASC", $chapter_id);
  while ($book_examples_data = db_fetch_object($book_examples_q))
  {
    $book_examples[$book_examples_data->id] = $book_examples_data->number . ' (' . $book_examples_data->caption . ')';
  }
  return $book_examples;
}

function _contributor_name($preference_id)
{
  $book_data = db_fetch_object(db_query("SELECT proposal.full_name as proposal_full_name FROM {textbook_companion_proposal} proposal LEFT JOIN {textbook_companion_preference} preference ON proposal.id = preference.proposal_id WHERE preference.id = %d", $preference_id));
  return $book_data->proposal_full_name;
}
