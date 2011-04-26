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

  /* add javascript for automatic book title, check if example uploaded, dependency selection effects */
  $chapter_name_js = " $(document).ready(function() {
    $('#edit-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/chapter_title/' + $('#edit-number').val() + '/' + " . $row->pre_id . ", function(data) {
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
    $('#edit-existing-depfile-dep-book-title').change(function() {
      var dep_selected = ''; 
      /* showing and hiding relevant files */
      $('.form-checkboxes .option').hide();
      $('.form-checkboxes .option').each(function(index) {
        var activeClass = $('#edit-existing-depfile-dep-book-title').val();
        if ($(this).children().hasClass(activeClass)) {
          $(this).show();
        }
        if ($(this).children().attr('checked') == true) {
          dep_selected += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
      $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-book-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-book-title').trigger('change');
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
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['example_warning'] = array(
    '#type' => 'item',
    '#title' => t('You should upload all the files (main or source files, result files, executable file if any)'),
    '#prefix' => '<div style="color:red">',
    '#suffix' => '</div>',
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
      '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
  );

  $form['dep_files'] = array(
    '#type' => 'item',
    '#title' => t('Dependency Files'),
  );

  /************ START OF EXISTING DEPENDENCIES **************/

  /* existing dependencies */
  $form['existing_depfile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Use Already Existing Dependency Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="existing-depfile-wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  /* existing dependencies */
  $form['existing_depfile']['selected'] = array(
    '#type' => 'item',
    '#title' => t('Existing Dependency Files Selected'),
    '#value' => '<div id="existing_depfile_selected"></div>',
  );

  $form['existing_depfile']['dep_book_title'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Book'),
      '#options' => _list_of_book_titles(),
  );

  list($files_options, $files_options_class) = _list_of_book_dependency_files();
  $form['existing_depfile']['dep_chapter_example_files'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Dependency Files'),
      '#options' => $files_options,
      '#options_class' => $files_options_class,
      '#multiple' => TRUE,
  );

  $form['existing_depfile']['dep_upload'] = array(
      '#type' => 'item',
      '#value' => l('Upload New Depedency Files', 'textbook_companion/code/upload_dep'),
  );
  /************ END OF EXISTING DEPENDENCIES **************/

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
      '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
  );
  $form['result']['result2'] = array(
      '#type' => 'file',
      '#title' => t('Upload result file'),
      '#size' => 48,
      '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
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
      '#description' => t('Allowed file extensions : ') . 'xcos,scicos',
  );
  $form['xcos']['xcos2'] = array(
      '#type' => 'file',
      '#title' => t('Upload xcos file'),
      '#size' => 48,
      '#description' => t('Allowed file extensions : ') . 'xcos,scicos',
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
  if (isset($_FILES['files']))
  {
    /* check if atleast one source or result file is uploaded */  
    if ( ! ($_FILES['files']['name']['sourcefile1'] || $_FILES['files']['name']['xcos1']))
      form_set_error('sourcefile1', t('Please upload atleast one main or source file or xcos file.'));

    /* check for valid filename extensions */  
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
    {
      if ($file_name)
      {
        /* checking file type */
        if (strstr($file_form_name, 'source'))
          $file_type = 'S';
        else if (strstr($file_form_name, 'result'))
          $file_type = 'R';
        else if (strstr($file_form_name, 'xcos'))
          $file_type = 'X';
        else
          $file_type = 'U';

        if ($file_type == 'X')
        {
          $allowed_extensions_str = 'xcos,scicos';
        } else {
          $allowed_extensions_str = variable_get('textbook_companion_extensions', '');
        }
        $allowed_extensions = explode(',' , $allowed_extensions_str);
        $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
        if (!in_array($temp_extension, $allowed_extensions))
          form_set_error($file_form_name, t('Only ' . $allowed_extensions_str . ' extensions can be uploaded.'));
        if ($_FILES['files']['size'][$file_form_name] <= 0)
          form_set_error($file_form_name, t('File size cannot be zero.'));

        /* check if valid file name */
        if (!textbook_companion_check_valid_filename($_FILES['files']['name'][$file_form_name]))
         form_set_error($file_form_name, t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));
      }
    }
  }

  /* add javascript again for automatic book title, check if example uploaded, dependency selection effects */
  $chapter_name_js = " $(document).ready(function() {
    $('#edit-number').change(function() {
      $.get('" . base_path() . "textbook_companion/ajax/chapter_title/' + $('#edit-number').val() + '/' + " . $row->pre_id . ", function(data) {
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
    $('#edit-existing-depfile-dep-book-title').change(function() {
      var dep_selected = ''; 
      /* showing and hiding relevant files */
      $('.form-checkboxes .option').hide();
      $('.form-checkboxes .option').each(function(index) {
        var activeClass = $('#edit-existing-depfile-dep-book-title').val();
        if ($(this).children().hasClass(activeClass)) {
          $(this).show();
        }
        if ($(this).children().attr('checked') == true) {
          dep_selected += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
      $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-book-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-book-title').trigger('change');
  });";
  drupal_add_js($chapter_name_js, 'inline', 'header');
}

function upload_examples_form_submit($form, &$form_state) {
  global $user;

  $base_path = $_SERVER['DOCUMENT_ROOT'] . base_path();
  $dest_path = 'uploads/';

  /* get book details */
  $book_result = db_query("SELECT pro.id as pro_id, pre.id as pre_id, pre.book as pre_book FROM {textbook_companion_proposal} pro JOIN {textbook_companion_preference} pre ON pro.id = pre.proposal_id WHERE pro.proposal_status = 1 AND pre.approval_status = 1 AND pro.uid = %d", $user->uid);
  $book_row = db_fetch_object($book_result);
  $preference_id = $book_row->pre_id;

  $dest_path .= $preference_id . '/';
  if (!is_dir($base_path . $dest_path))
    mkdir($base_path . $dest_path);

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

  /*  get example details - dont allow if already example present and status is approved */
  $cur_example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d and number = %d", $chapter_id, $form_state['values']['example_number']);
  if ($cur_example_d = db_fetch_object($cur_example_q))
  {
    if ($cur_example_d->approval_status == 1)
    {
      drupal_set_message(t("Example already approved. Cannot overwrite it."), 'error');
      drupal_goto('textbook_companion/code');
      return;
    } else {
      /* deleting old example files */
      $cur_examples_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $cur_example_d->id);
      while ($cur_examples_files_data = db_fetch_object($cur_examples_files_q))
      {
        if (!unlink($base_path . $cur_examples_files_data->filepath))
          drupal_set_message(t('Error deleting !file', array('!file' => $cur_examples_files_data->filepath)), 'error');
      }
      /* deleting old example database entries */
      db_query("DELETE FROM {textbook_companion_example_files} WHERE example_id = %d", $cur_example_d->id);
      db_query("DELETE FROM {textbook_companion_example} WHERE id = %d", $cur_example_d->id);
      /* deleting old dependency database entries */
      db_query("DELETE FROM {textbook_companion_example_dependency} WHERE example_id = %d", $cur_example_d->id);
      drupal_set_message(t("Example overwritten."), 'status');
    }
  }

  /* creating directories */
  $dest_path .= 'CH' . $form_state['values']['number'] . '/';
  if (!is_dir($base_path . $dest_path))
    mkdir($base_path . $dest_path);

  $dest_path .= 'EX' . $form_state['values']['example_number'] . '/';
  if (!is_dir($base_path . $dest_path))
    mkdir($base_path . $dest_path);

  /* creating example database entry */
  db_query("INSERT INTO {textbook_companion_example} (chapter_id, number, caption, approval_status, timestamp) VALUES (%d, '%s', '%s', %d, %d)",
    $chapter_id,
    $form_state['values']['example_number'],
    $form_state['values']['example_caption'],
    0,
    time()
  );
  $example_id = db_last_insert_id('textbook_companion_example', 'id');

  /* linking existing dependencies */
  foreach ($form_state['values']['existing_depfile']['dep_chapter_example_files'] as $row)
  {
    if ($row > 0)
    {
      /* insterting into database */
      db_query("INSERT INTO {textbook_companion_example_dependency} (example_id, dependency_id, approval_status, timestamp)
        VALUES (%d, %d, %d, %d)",
        $example_id,
        $row,
        0,
        time()
      );
    }
  }

  /* uploading files */
  foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
  {
    if ($file_name)
    {
      /* checking file type */
      if (strstr($file_form_name, 'source'))
        $file_type = 'S';
      else if (strstr($file_form_name, 'result'))
        $file_type = 'R';
      else if (strstr($file_form_name, 'xcos'))
        $file_type = 'X';
      else
        $file_type = 'U';

      /* uploading file */
      if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $base_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
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

  /* sending email */
  $email_to = $user->mail . ', ' . variable_get('textbook_companion_emails', '');
  $param['example_uploaded']['example_id'] = $example_id;
  $param['example_uploaded']['user_id'] = $user->uid;
  if (!drupal_mail('textbook_companion', 'example_uploaded', $email_to, language_default(), $param, NULL, TRUE))
    drupal_set_message('Error sending email message.', 'error');

  drupal_goto('textbook_companion/code');
}

function _upload_examples_delete()
{
  global $user;
  $example_id = arg(3);

  /* check if example exists */
  if (!$preference_data = db_fetch_object(db_query("SELECT preference.id as preference_id FROM {textbook_companion_proposal} proposal JOIN {textbook_companion_preference} preference ON proposal.id = preference.proposal_id WHERE proposal.uid = %d AND preference.approval_status = 1 ORDER BY preference.id DESC LIMIT 1", $user->uid)))
  {
    drupal_set_message('You do not have permission to delete this example.', 'error');
    drupal_goto('textbook_companion/code');
    return;
  }
  if (!$example_data = db_fetch_object(db_query("SELECT example.approval_status as example_approval_status FROM {textbook_companion_chapter} chapter JOIN {textbook_companion_example} example ON chapter.id = example.chapter_id WHERE chapter.preference_id = %d AND example.id = %d ORDER BY example.id DESC LIMIT 1", $preference_data->preference_id, $example_id)))
  {
    drupal_set_message('You do not have permission to delete this example.', 'error');
    drupal_goto('textbook_companion/code');
    return;
  }
  if ($example_data->example_approval_status != 0)
  {
    drupal_set_message('You cannnot delete an example after it has been approved.', 'error');
    drupal_goto('textbook_companion/code');
    return;
  }

  /* deleting example files */
  $cur_examples_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);
  while ($cur_examples_files_data = db_fetch_object($cur_examples_files_q))
  {
    if (!unlink($cur_examples_files_data->filepath))
      drupal_set_message(t('Error deleting !file', array('!file' => $cur_examples_files_data->filepath)), 'error');
  }
  /* deleting example database entries */
  db_query("DELETE FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);
  db_query("DELETE FROM {textbook_companion_example} WHERE id = %d", $example_id);
  /* deleting dependency database entries */
  db_query("DELETE FROM {textbook_companion_example_dependency} WHERE example_id = %d", $example_id);

  drupal_set_message('Example deleted.', 'status');
  drupal_goto('textbook_companion/code');
  return;
}

function _list_of_book_titles()
{
  $book_titles = array('0' => 'Please select...');
  $book_titles_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 OR approval_status = 3 ORDER BY book ASC");
  while ($book_titles_data = db_fetch_object($book_titles_q))
  {
    $book_titles[$book_titles_data->id] = $book_titles_data->book . ' (Written by ' . $book_titles_data->author . ')';
  }
  return $book_titles;
}

function _list_of_book_dependency_files()
{
  $book_dependency_files = array();
  $book_dependency_files_class = array();
  $book_dependency_files_q = db_query("SELECT * FROM {textbook_companion_dependency_files} ORDER BY filename ASC");

  while ($book_dependency_files_data = db_fetch_object($book_dependency_files_q))
  {
    $temp_caption = '';
    if ($book_dependency_files_data->caption)
      $temp_caption .= ' (' . $book_dependency_files_data->caption . ')';
    $book_dependency_files[$book_dependency_files_data->id] = l($book_dependency_files_data->filename . $temp_caption, 'download/dependency/' . $book_dependency_files_data->id);
    $book_dependency_files_class[$book_dependency_files_data->id] = $book_dependency_files_data->preference_id;
  }
  return array($book_dependency_files, $book_dependency_files_class);
}

