<?php
// $Id$

function upload_examples()
{
  return drupal_get_form('upload_examples_form');
}

function upload_examples_form($form_state)
{
  global $user;

  /* get approved book details */
  $result = db_query("SELECT pro.id as pro_id, pre.id as pre_id, pre.book as pre_book, pro.full_name as pro_full_name FROM {textbook_companion_proposal} pro JOIN {textbook_companion_preference} pre ON pro.id = pre.proposal_id WHERE pro.proposal_status = 1 AND pre.approval_status = 1 AND pro.uid = %d", $user->uid);
  $row = db_fetch_object($result);
  if (!$row)
  {
    drupal_set_message("Your book proposal has not been approved. You will receive a email from us when your proposal is approved.", 'error');
    drupal_goto('');
    return;
  }

  /* add javascript for book title */
  $chapter_name_js = " $(document).ready(function() {
    $('#edit-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/chapter_title/' + $('#edit-number').val(), function(data) {
        $('#edit-name').val(data);
      });
    });
    $('#edit-example-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/example_exists/' + $('#edit-number').val() + '/' + $('#edit-example-number').val(), function(data) {
        if (data) {
          alert(data);
        }
      });
    });
  });";
  drupal_add_js($chapter_name_js, 'inline', 'header');

  $form['#attributes'] = array('enctype' => "multipart/form-data");

  $form['book_details']['book'] = array(
    '#type' => 'item',
    '#value' => $row->pre_book,
    '#title' => t('Title of the Book'),
  );
  $form['contributor_name'] = array(
    '#type' => 'item',
    '#value' => $row->pro_full_name,
    '#title' => t('Contributor Name'),
  );

  $form['number'] = array(
    '#type' => 'select',
    '#title' => t('Chapter No'),
    '#options' => array('' => '(Select)', '1' => '1', '2', '3', '4', '5', '6', '7', '8', '9', '10',
                        '11', '12', '13', '14', '15', '16', '17', '18', '19', '20',
                        '21', '22', '23', '24', '25', '26', '27', '28', '29', '30'),
    '#multiple' => FALSE,
    '#size' => 1,
    '#required' => TRUE,
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the Chapter'),
    '#size' => 40,
    '#maxlength' => 100,
    '#required' => TRUE,
  );
  $form['example_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Example No'),
    '#size' => 5,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  $form['example_caption'] = array(
    '#type' => 'textfield',
    '#title' => t('Caption'),
    '#size' => 40,
    '#maxlength' => 100,
    '#required' => TRUE,
  );
  $form['example_warning'] = array(
    '#type' => 'item',
    '#title' => t('You should upload all the files (main or source files, dependencies, result files, executable file if any)'),
    '#prefix' => '<div style="color:red">',
    '#suffix' => '</div>',
  );

  /* dummy */
  $form['depfilex'] = array(
    '#type' => 'fieldset',
    '#title' => t('Use already existing dependency files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['depfilex']['dummy1'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Book'),
      '#options' => array('Ubuntu Guide (Written by Mr. Allex)', 'Introduction to Scilab (Written by Mr. Brown)', 'Drupal Development (Written by Mr. Smal)'),
  );
  $form['depfilex']['dummy2'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Chapter'),
      '#options' => array('1. Introduction', '2. Installation', '3. Configuration'),
  );
  $form['depfilex']['dummy3'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Dependency Files'),
      '#options' => array(l('one.dep', ''), l('two.dep', ''), l('three.dep', ''), l('four.dep', '')),
      '#multiple' => TRUE,
      '#size' => 5,
  );
  $form['depfilex']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Upload')
  );

  $form['depfile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dependency Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['depfile']['depfile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file'),
      '#size' => 48,
      '#description' => t('Upload dependency file of the example.'),
  );
  $form['depfile']['depfile2'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file'),
      '#size' => 48,
      '#description' => t('Upload dependency file of the example.'),
  );
  $form['depfile']['depfile3'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file'),
      '#size' => 48,
      '#description' => t('Upload dependency file of the example.'),
  );
  $form['depfile']['depfile4'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file'),
      '#size' => 48,
      '#description' => t('Upload dependency file of the example.'),
  );

  $form['sourcefile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Main or Source Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['sourcefile']['sourcefile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload main or source file'),
      '#size' => 48,
      '#description' => t('Upload main or source file of the example.'),
  );
  $form['sourcefile']['sourcefile2'] = array(
      '#type' => 'file',
      '#title' => t('Upload main or source file'),
      '#size' => 48,
      '#description' => t('Upload main or source file of the example.'),
  );
  $form['sourcefile']['sourcefile3'] = array(
      '#type' => 'file',
      '#title' => t('Upload main or source file'),
      '#size' => 48,
      '#description' => t('Upload main or source file of the example.'),
  );

  $form['result'] = array(
    '#type' => 'fieldset',
    '#title' => t('Result Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['result']['result1'] = array(
      '#type' => 'file',
      '#title' => t('Upload result file'),
      '#size' => 48,
      '#description' => t('Upload result file of the example.'),
  );
  $form['result']['result2'] = array(
      '#type' => 'file',
      '#title' => t('Upload result file'),
      '#size' => 48,
      '#description' => t('Upload result file of the example.'),
  );

  $form['xcos'] = array(
    '#type' => 'fieldset',
    '#title' => t('XCOS Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['xcos']['xcos1'] = array(
      '#type' => 'file',
      '#title' => t('Upload xcos file'),
      '#size' => 48,
      '#description' => t('Upload xcos file of the example.'),
  );
  $form['xcos']['xcos2'] = array(
      '#type' => 'file',
      '#title' => t('Upload xcos file'),
      '#size' => 48,
      '#description' => t('Upload xcos file of the example.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'textbook_companion/code'),
  );
  return $form;
}

function upload_examples_form_validate($form, &$form_state)
{
}

function upload_examples_form_submit($form, &$form_state) {
  global $user;

  $dest_path = $_SERVER['DOCUMENT_ROOT'] . base_path() . 'uploads/';

  /* get book details */
  $book_result = db_query("SELECT pro.id as pro_id, pre.id as pre_id, pre.book as pre_book FROM {textbook_companion_proposal} pro JOIN {textbook_companion_preference} pre ON pro.id = pre.proposal_id WHERE pro.proposal_status = 1 AND pre.approval_status = 1 AND pro.uid = %d", $user->uid);
  $book_row = db_fetch_object($book_result);
  $preference_id = $book_row->pre_id;
  $dest_path .= $preference_id . '/';
  if (!is_dir($dest_path))
    mkdir($dest_path);

  /* inserting chapter details */
  $chapter_id = 0;
  $chapter_result = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d AND number = %d", $preference_id, $form_state['values']['number']);
  if (!$chapter_row = db_fetch_object($chapter_result))
  {
    db_query("INSERT INTO {textbook_companion_chapter} (preference_id, number, name) VALUES (%d, '%s', '%s')",
      $preference_id,
      $form_state['values']['number'],
      $form_state['values']['name']
    );
    $chapter_id = db_last_insert_id('textbook_companion_chapter', 'id');
  } else {
    $chapter_id = $chapter_row->id;
  }

  /*  get example details - dont allow if already example present */
  $cur_example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d and number = %d", $chapter_id, $form_state['values']['example_number']);
  if ($cur_example_d = db_fetch_object($cur_example_q))
  {
    drupal_set_message(t("Example already uploaded."), 'error');
    return;
  }

  /* creating directories */
  $dest_path .= 'CH' . $form_state['values']['number'] . '/';
  if (!is_dir($dest_path))
    mkdir($dest_path);

  $dest_path .= 'EX' . $form_state['values']['example_number'] . '/';
  if (!is_dir($dest_path))
    mkdir($dest_path);

  /* creating example entry */
  db_query("INSERT INTO {textbook_companion_example} (chapter_id, number, caption, approval_status, timestamp) VALUES (%d, '%s', '%s', %d, %d)",
    $chapter_id,
    $form_state['values']['example_number'],
    $form_state['values']['example_caption'],
    0,
    time()
  );
  $example_id = db_last_insert_id('textbook_companion_example', 'id');

  /* uploading files */
  foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
  {
    if ($file_name)
    {
      /* checking file type */
      if (strstr($file_form_name, 'dep'))
        $file_type = 'D';
      else if (strstr($file_form_name, 'source'))
        $file_type = 'S';
      else if (strstr($file_form_name, 'result'))
        $file_type = 'R';
      else
        $file_type = 'U';

      /* uploading file */
      if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        /* for uploaded files making an entry in the database */
        db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, timestamp)
          VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
          $example_id,
          $_FILES['files']['name'][$file_form_name],
          $dest_path . $_FILES['files']['name'][$file_form_name],
          $_FILES['files']['type'][$file_form_name],
          $_FILES['files']['size'][$file_form_name],
          $file_type,
          time()
          );
        drupal_set_message($file_name . ' uploaded successfully.', 'status');
      } else {
        drupal_set_message('Error uploading file : ' . $dest_path . '/' . $_FILES['files']['name'][$file_form_name], 'error');
      }
    }
  }
  drupal_set_message('Example uploaded successfully.', 'status');
  drupal_goto('textbook_companion/code');
}

