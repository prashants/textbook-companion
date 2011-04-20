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
    '#title' => t('You should upload all the files (main or source files, dependencies, result files, executable file if any)'),
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
      '#description' => t('Upload main or source file of the example.'),
  );
  $form['sourcefile']['sourcefile2'] = array(
      '#type' => 'file',
      '#title' => t('Upload main or source file'),
      '#size' => 48,
      '#description' => t('Upload main or source file of the example.'),
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

  /************ END OF EXISTING DEPENDENCIES **************/

  $form['depfile'] = array(
    '#type' => 'fieldset',
    '#title' => t('New Dependency Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['depfile']['depfile1_caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption for dependency file 1'),
      '#size' => 15,
  );
  $form['depfile']['depfile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file 1'),
      '#size' => 48,
  );
  $form['depfile']['depfile2_caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption for dependency file 2'),
      '#size' => 15,
  );
  $form['depfile']['depfile2'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file 2'),
      '#size' => 48,
  );
  $form['depfile']['depfile3_caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption for dependency file 3'),
      '#size' => 15,
  );
  $form['depfile']['depfile3'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file 3'),
      '#size' => 48,
  );
  $form['depfile']['depfile4_caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption for dependency file 4'),
      '#size' => 15,
  );
  $form['depfile']['depfile4'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file 4'),
      '#size' => 48,
  );
  $form['depfile']['depfile5_caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption for dependency file 5'),
      '#size' => 15,
  );
  $form['depfile']['depfile5'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file 5'),
      '#size' => 48,
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
  );
  $form['result']['result2'] = array(
      '#type' => 'file',
      '#title' => t('Upload result file'),
      '#size' => 48,
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
  );
  $form['xcos']['xcos2'] = array(
      '#type' => 'file',
      '#title' => t('Upload xcos file'),
      '#size' => 48,
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
    if ( ! ($_FILES['files']['name']['sourcefile1'] || $_FILES['files']['name']['xcos1']))
      form_set_error('sourcefile1', t('Please upload atleast one main or source file or xcos file.'));

    $allowed_extensions = explode(',' , variable_get('textbook_companion_extensions', ''));
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
    {
      if ($file_name)
      {
         if (!in_array(end(explode('.', strtolower($_FILES['files']['name'][$file_form_name]))), $allowed_extensions))
           form_set_error($file_form_name, t('Only ' . variable_get('textbook_companion_extensions', '') . ' extensions can be uploaded.'));
         if ($_FILES['files']['size'][$file_form_name] <= 0)
           form_set_error($file_form_name, t('File size is zero.'));
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
  $dest_dep_path = 'uploads/';

  /* get book details */
  $book_result = db_query("SELECT pro.id as pro_id, pre.id as pre_id, pre.book as pre_book FROM {textbook_companion_proposal} pro JOIN {textbook_companion_preference} pre ON pro.id = pre.proposal_id WHERE pro.proposal_status = 1 AND pre.approval_status = 1 AND pro.uid = %d", $user->uid);
  $book_row = db_fetch_object($book_result);
  $preference_id = $book_row->pre_id;

  $dest_path .= $preference_id . '/';
  $dest_dep_path .= $preference_id . '/' . 'DEPENDENCIES' . '/';
  if (!is_dir($base_path . $dest_path))
    mkdir($base_path . $dest_path);
  if (!is_dir($base_path . $dest_dep_path))
    mkdir($base_path . $dest_dep_path);

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

  /* creating example entry */
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
      /* TODO : copying file */
      
      /* insterting into database */
      $existing_dependency_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_example_files} WHERE id = %d", $row));
      db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, caption, timestamp)
        VALUES (%d, '%s', '%s', '%s', %d, '%s', '%s', %d)",
        $example_id,
        $existing_dependency_data->filename,
        $existing_dependency_data->filepath,
        $existing_dependency_data->filemime,
        $existing_dependency_data->filesize,
        'D',
        $existing_dependency_data->caption,
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
      if (strstr($file_form_name, 'dep'))
        $file_type = 'D';
      else if (strstr($file_form_name, 'source'))
        $file_type = 'S';
      else if (strstr($file_form_name, 'result'))
        $file_type = 'R';
      else if (strstr($file_form_name, 'xcos'))
        $file_type = 'X';
      else
        $file_type = 'U';

      /* uploading file */
      if ($file_type == 'D')
      {
        if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $base_path . $dest_dep_path . $_FILES['files']['name'][$file_form_name]))
        {
          /* for uploaded files making an entry in the database */
          db_query("INSERT INTO {textbook_companion_example_files} (example_id, filename, filepath, filemime, filesize, filetype, caption, timestamp)
            VALUES (%d, '%s', '%s', '%s', %d, '%s', '%s', %d)",
            $example_id,
            $_FILES['files']['name'][$file_form_name],
            $dest_dep_path . $_FILES['files']['name'][$file_form_name],
            $_FILES['files']['type'][$file_form_name],
            $_FILES['files']['size'][$file_form_name],
            $file_type,
            $form_state['values'][$file_form_name . '_caption'],
            time()
            );
          drupal_set_message($file_name . ' uploaded successfully.', 'status');
        } else {
          drupal_set_message('Error uploading file : ' . $dest_path . '/' . $_FILES['files']['name'][$file_form_name], 'error');
        }
      } else {
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
  }
  drupal_set_message('Example uploaded successfully.', 'status');
  drupal_goto('textbook_companion/code');
}

/*
function upload_examples_edit_form($form_state)
{
  global $user;
  $example_id = arg(3);

  // check if example exists
  if (!$preference_data = db_fetch_object(db_query("SELECT preference.id as preference_id FROM {textbook_companion_proposal} proposal JOIN {textbook_companion_preference} preference ON proposal.id = preference.proposal_id WHERE proposal.uid = %d AND preference.approval_status = 1 ORDER BY preference.id DESC LIMIT 1", $user->uid)))
  {
    drupal_set_message('You do not have permission to edit this example.', 'error');
    drupal_goto('textbook_companion/code');
    return;
  }
  if (!$example_data = db_fetch_object(db_query("SELECT example.approval_status as example_approval_status FROM {textbook_companion_chapter} chapter JOIN {textbook_companion_example} example ON chapter.id = example.chapter_id WHERE chapter.preference_id = %d AND example.id = %d ORDER BY example.id DESC LIMIT 1", $preference_data->preference_id, $example_id)))
  {
    drupal_set_message('You do not have permission to edit this example.', 'error');
    drupal_goto('textbook_companion/code');
    return;
  }
  if ($example_data->example_approval_status != 0)
  {
    drupal_set_message('You cannnot edit an example after it has been approved.', 'error');
    drupal_goto('textbook_companion/code');
    return;
  }

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
*/

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

  /* deleting old example files */
  $cur_examples_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);
  while ($cur_examples_files_data = db_fetch_object($cur_examples_files_q))
  {
    if (!unlink($cur_examples_files_data->filepath))
      drupal_set_message(t('Error deleting !file', array('!file' => $cur_examples_files_data->filepath)), 'error');
  }
  /* deleting old example database entries */
  db_query("DELETE FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);
  db_query("DELETE FROM {textbook_companion_example} WHERE id = %d", $example_id);

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
  $book_dependency_files_q = db_query("SELECT chapter.preference_id as chapter_preference_id, files.id as files_id, files.filename as files_filename, files.caption as files_caption
    FROM {textbook_companion_chapter} chapter
    LEFT JOIN {textbook_companion_example} example ON chapter.id = example.chapter_id
    LEFT JOIN {textbook_companion_example_files} files ON example.id = files.example_id
    WHERE files.filetype = 'D' ORDER BY files.filename ASC");

  while ($book_dependency_files_data = db_fetch_object($book_dependency_files_q))
  {
    $temp_caption = '';
    if ($book_dependency_files_data->caption)
      $temp_caption .= ' (' . $book_dependency_files_data->files_caption . ')';
    $book_dependency_files[$book_dependency_files_data->files_id] = l($book_dependency_files_data->files_filename . $temp_caption, '');
    $book_dependency_files_class[$book_dependency_files_data->files_id] = $book_dependency_files_data->chapter_preference_id;
  }
  return array($book_dependency_files, $book_dependency_files_class);
}

function theme_upload_examples_form($form) {
  drupal_add_js('alert("hi");', 'inline', 'header');
}
