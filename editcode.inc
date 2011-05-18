<?php
// $Id$

/******************************************************************************/
/***************************** EDIT EXAMPLE ***********************************/
/******************************************************************************/

function upload_examples_edit_form($form_state)
{
  global $user;
  $example_id = arg(3);

  /* get example details */
  $example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $example_id);
  $example_data = db_fetch_object($example_q);
  if (!$example_q)
  {
    drupal_set_message(t("Invalid example selected."), 'error');
    drupal_goto('');
    return;
  }
  if ($example_data->approval_status != 0)
  {
    drupal_set_message(t("You cannot edit an example after it has been approved or dis-approved. Please contact site administrator if you want to edit this example."), 'error');
    drupal_goto('');
    return;
  }

  // /* get examples files */
  // $source_file = "";
  // $result1_file = "";
  // $result2_file = "";
  // $xcos1_file = "";
  // $xcos2_file = "";
// 
  // $example_files_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $example_id);
  // while ($example_files_data = db_fetch_object($example_files_q))
  // {
    // if ($example_files_data->filetype == "S")
      // $source_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id); 
    // if ($example_files_data->filetype == "R")
    // {
      // if (strlen($result1_file) == 0)
        // $result1_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
      // else
        // $result2_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
    // }
    // if ($example_files_data->filetype == "X")
    // {
      // if (strlen($xcos1_file) <= 0)
        // $xcos1_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
      // else
        // $xcos2_file = l($example_files_data->filename, 'download/file/' . $example_files_data->id);
    // }
  // }

  /* get chapter details */
  $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
  $chapter_data = db_fetch_object($chapter_q);
  if (!$chapter_data)
  {
    drupal_set_message(t("Invalid chapter selected."), 'error');
    drupal_goto('');
    return;
  }

  /* get preference details */
  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $chapter_data->preference_id);
  $preference_data = db_fetch_object($preference_q);
  if (!$preference_data)
  {
    drupal_set_message(t("Invalid book selected."), 'error');
    drupal_goto('');
    return;
  }
  if ($preference_data->approval_status != 1)
  {
    drupal_set_message(t("Cannot edit example. Either the book proposal has not been approved or it has been rejected."), 'error');
    drupal_goto('');
    return;
  }

  /* get proposal details */
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message(t("Invalid proposal selected."), 'error');
    drupal_goto('');
    return;
  }
  if ($proposal_data->uid != $user->uid)
  {
    drupal_set_message(t("You do not have permissions to edit this example."), 'error');
    drupal_goto('');
    return;
  }

  $user_data = user_load($proposal_data->uid);

  /* add javascript for automatic book title, check if example uploaded, dependency selection effects */
  $chapter_name_js = " $(document).ready(function() {
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

  $form['#redirect'] = 'textbook_companion/code';
  $form['#attributes'] = array('enctype' => "multipart/form-data");

  $form['book_details']['book'] = array(
    '#type' => 'item',
    '#value' => $preference_data->book,
    '#title' => t('Title of the Book'),
  );
  $form['contributor_name'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->full_name,
    '#title' => t('Contributor Name'),
  );
  $form['number'] = array(
    '#type' => 'item',
    '#title' => t('Chapter No'),
    '#value' => $chapter_data->number,
  );
  $form['name'] = array(
    '#type' => 'item',
    '#title' => t('Title of the Chapter'),
    '#default_value' => $chapter_data->name,
  );
  $form['example_number'] = array(
    '#type' => 'item',
    '#title' => t('Example No'),
    '#value' => $example_data->number,
  );
  $form['example_caption'] = array(
    '#type' => 'textfield',
    '#title' => t('Caption'),
    '#size' => 40,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $example_data->caption,
  );
  // $form['example_warning'] = array(
    // '#type' => 'item',
    // '#title' => t('You should upload all the files (main or source files, result files, executable file if any)'),
    // '#prefix' => '<div style="color:red">',
    // '#suffix' => '</div>',
  // );
// 
  // $form['sourcefile'] = array(
    // '#type' => 'fieldset',
    // '#title' => t('Main or Source Files'),
    // '#collapsible' => FALSE,
    // '#collapsed' => FALSE,
  // );
  // if ($source_file)
  // {
    // $form['sourcefile']['cur_source'] = array(
        // '#type' => 'item',
        // '#title' => t('Existing Main or Source File'),
        // '#value' => $source_file,
    // );
    // $form['sourcefile']['cur_source_checkbox'] = array(
        // '#type' => 'checkbox',
        // '#title' => t('Delete Existing Main or Source File'),
        // '#description' => 'Check to delete the existing Main or Source file.',
    // );
    // $form['sourcefile']['sourcefile1'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New Main or Source File'),
        // '#size' => 48,
        // '#description' => t("Upload new Main or Source file above if you want to replace the existing file, leave blank if you want to keep using the existing file. <br />") .
        // t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
    // );
  // } else {
    // $form['sourcefile']['sourcefile1'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New Main or Source File'),
        // '#size' => 48,
        // '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
    // );
  // }
// 
  // $form['dep_files'] = array(
    // '#type' => 'item',
    // '#title' => t('Dependency Files'),
  // );
// 
  // /************ START OF EXISTING DEPENDENCIES **************/
// 
  // $dependency_files = array();
  // $dependency_q = db_query("SELECT * FROM {textbook_companion_example_dependency} WHERE example_id = %d", $example_data->id);
  // while ($dependency_data = db_fetch_object($dependency_q))
  // {
    // $dependency_files[] = $dependency_data->dependency_id;
  // }
// 
  // /* existing dependencies */
  // $form['existing_depfile'] = array(
    // '#type' => 'fieldset',
    // '#title' => t('Use Already Existing Dependency Files'),
    // '#collapsible' => FALSE,
    // '#collapsed' => FALSE,
    // '#prefix' => '<div id="existing-depfile-wrapper">',
    // '#suffix' => '</div>',
    // '#tree' => TRUE,
  // );
// 
  // /* existing dependencies */
  // $form['existing_depfile']['selected'] = array(
    // '#type' => 'item',
    // '#title' => t('Existing Dependency Files Selected'),
    // '#value' => '<div id="existing_depfile_selected"></div>',
  // );
// 
  // $form['existing_depfile']['dep_book_title'] = array(
      // '#type' => 'select',
      // '#title' => t('Title of the Book'),
      // '#options' => _list_of_book_titles(),
  // );
// 
  // list($files_options, $files_options_class) = _list_of_book_dependency_files();
  // $form['existing_depfile']['dep_chapter_example_files'] = array(
      // '#type' => 'checkboxes',
      // '#title' => t('Dependency Files'),
      // '#options' => $files_options,
      // '#options_class' => $files_options_class,
      // '#multiple' => TRUE,
      // '#default_value' => $dependency_files,
  // );
// 
  // $form['existing_depfile']['dep_upload'] = array(
      // '#type' => 'item',
      // '#value' => l('Upload New Depedency Files', 'textbook_companion/code/upload_dep'),
  // );
  // /************ END OF EXISTING DEPENDENCIES **************/
// 
  // $form['result'] = array(
    // '#type' => 'fieldset',
    // '#title' => t('Result Files'),
    // '#collapsible' => FALSE,
    // '#collapsed' => FALSE,
  // );
  // if ($result1_file)
  // {
    // $form['result']['cur_result1'] = array(
        // '#type' => 'item',
        // '#title' => t('Existing Result File 1'),
        // '#value' => $result1_file,
    // );
    // $form['result']['cur_result1_checkbox'] = array(
        // '#type' => 'checkbox',
        // '#title' => t('Delete Existing Result File 1'),
        // '#description' => 'Check to delete the existing Result file.',
    // );
    // $form['result']['result1'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New Result File 1'),
        // '#size' => 48,
        // '#description' => t("Upload new Result file above if you want to replace the existing file, leave blank if you want to keep using the existing file. <br />") .
        // t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
    // );
  // } else {
    // $form['result']['result1'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New Result File 1'),
        // '#size' => 48,
        // '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
    // );
  // }
// 
  // $form['result']['br'] = array(
      // '#type' => 'item',
      // '#value' => "<br />",
  // );
// 
  // if ($result2_file)
  // {
    // $form['result']['cur_result2'] = array(
        // '#type' => 'item',
        // '#title' => t('Existing Result File 2'),
        // '#value' => $result2_file,
    // );
    // $form['result']['cur_result2_checkbox'] = array(
        // '#type' => 'checkbox',
        // '#title' => t('Delete Existing Result File 2'),
        // '#description' => 'Check to delete the existing Result file.',
    // );
    // $form['result']['result2'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New Result file 2'),
        // '#size' => 48,
        // '#description' => t("Upload new Result file above if you want to replace the existing file, leave blank if you want to keep using the existing file. <br />") . 
        // t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
    // );
  // } else {
    // $form['result']['result2'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New Result file 2'),
        // '#size' => 48,
        // '#description' => t('Allowed file extensions : ') . variable_get('textbook_companion_extensions', ''),
    // );
  // }
  // $form['xcos'] = array(
    // '#type' => 'fieldset',
    // '#title' => t('XCOS Files'),
    // '#collapsible' => FALSE,
    // '#collapsed' => FALSE,
  // );
  // if ($xcos1_file)
  // {
    // $form['xcos']['cur_xcos1'] = array(
        // '#type' => 'item',
        // '#title' => t('Existing xcos File 1'),
        // '#value' => $xcos1_file,
    // );
    // $form['xcos']['cur_xcos1_checkbox'] = array(
        // '#type' => 'checkbox',
        // '#title' => t('Delete Existing xcos File 1'),
        // '#description' => 'Check to delete the existing xcos file.',
    // );
    // $form['xcos']['xcos1'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New xcos file 1'),
        // '#size' => 48,
        // '#description' => t("Upload new xcos file above if you want to replace the existing file, leave blank if you want to keep using the existing file. <br />") .
        // t('Allowed file extensions : ') . 'xcos,scicos',
    // );
  // } else {
    // $form['xcos']['xcos1'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New xcos file 1'),
        // '#size' => 48,
        // '#description' => t('Allowed file extensions : ') . 'xcos,scicos',
    // );
  // }
// 
  // $form['xcos']['br'] = array(
      // '#type' => 'item',
      // '#value' => "<br />",
  // );
  // 
  // if ($xcos2_file)
  // {
    // $form['xcos']['cur_xcos2'] = array(
        // '#type' => 'item',
        // '#title' => t('Existing xcos File 2'),
        // '#value' => $xcos2_file,
    // );
    // $form['xcos']['cur_xcos2_checkbox'] = array(
        // '#type' => 'checkbox',
        // '#title' => t('Delete Existing xcos File 2'),
        // '#description' => 'Check to delete the existing xcos file.',
    // );
    // $form['xcos']['xcos2'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New xcos file 2'),
        // '#size' => 48,
        // '#description' =>  t("Upload new xcos file above if you want to replace the existing file, leave blank if you want to keep using the existing file. <br />") . 
        // t('Allowed file extensions : ') . 'xcos,scicos',
    // );
  // } else {
    // $form['xcos']['xcos2'] = array(
        // '#type' => 'file',
        // '#title' => t('Upload New xcos file 2'),
        // '#size' => 48,
        // '#description' => t('Allowed file extensions : ') . 'xcos,scicos',
    // );
  // }

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

function upload_examples_edit_form_submit($form, &$form_state)
{
  global $user;
  $example_id = arg(3);

  /* get example details */
  $example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $example_id);
  $example_data = db_fetch_object($example_q);
  if (!$example_q)
  {
    drupal_set_message(t("Invalid example selected."), 'error');
    drupal_goto('');
    return;
  }
  if ($example_data->approval_status != 0)
  {
    drupal_set_message(t("You cannot edit an example after it has been approved or dis-approved. Please contact site administrator if you want to edit this example."), 'error');
    drupal_goto('');
    return;
  }

  /* get chapter details */
  $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $example_data->chapter_id);
  $chapter_data = db_fetch_object($chapter_q);
  if (!$chapter_data)
  {
    drupal_set_message(t("Invalid chapter selected."), 'error');
    drupal_goto('');
    return;
  }

  /* get preference details */
  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $chapter_data->preference_id);
  $preference_data = db_fetch_object($preference_q);
  if (!$preference_data)
  {
    drupal_set_message(t("Invalid book selected."), 'error');
    drupal_goto('');
    return;
  }
  if ($preference_data->approval_status != 1)
  {
    drupal_set_message(t("Cannot edit example. Either the book proposal has not been approved or it has been rejected."), 'error');
    drupal_goto('');
    return;
  }

  /* get proposal details */
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message(t("Invalid proposal selected."), 'error');
    drupal_goto('');
    return;
  }
  if ($proposal_data->uid != $user->uid)
  {
    drupal_set_message(t("You do not have permissions to edit this example."), 'error');
    drupal_goto('');
    return;
  }

  db_query("UPDATE {textbook_companion_example} SET caption = '%s' WHERE id = %d", $form_state['values']['example_caption'], $example_id);

  drupal_set_message(t("Example successfully udpated."), 'status');
}


/************************** EDIT CHAPTER TITLE ********************************/

function edit_chapter_title_form($form_state)
{
  global $user;

  /************************ start approve book details ************************/
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('textbook_companion/code');
  }
  if ($proposal_data->proposal_status  != 1)
  {
    switch ($proposal_data->proposal_status )
    {
    case 0:
      drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
      drupal_goto('textbook_companion/code');
      return;
      break;
    case 2:
      drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'textbook_companion/proposal') . '.'), 'error');
      drupal_goto('textbook_companion/code');
      return;
      break;
    case 3:
      drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'textbook_companion/proposal') . '.'), 'status');
      drupal_goto('textbook_companion/code');
      return;
      break;
    default:
       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
       drupal_goto('textbook_companion/code');
       return;
       break;
    }
  }

  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
  $preference_data = db_fetch_object($preference_q);
  if (!$preference_data)
  {
    drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
    drupal_goto('textbook_companion/code');
    return;
  }
  /************************ end approve book details **************************/

  $chapter_id = arg(4);

  $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d AND preference_id = %d", $chapter_id, $preference_data->id);
  $chapter_data = db_fetch_object($chapter_q);
  if (!$chapter_data)
  {
    drupal_set_message(t('Invalid chapter.'), 'error');
    drupal_goto('textbook_companion/code');
    return;
  }

  $form['#redirect'] = 'textbook_companion/code';

  $form['book_details']['book'] = array(
    '#type' => 'item',
    '#value' => $preference_data->book,
    '#title' => t('Title of the Book'),
  );
  $form['contributor_name'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->full_name,
    '#title' => t('Contributor Name'),
  );
  $form['number'] = array(
    '#type' => 'item',
    '#title' => t('Chapter No'),
    '#value' => $chapter_data->number,
  );

  $form['chapter_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the Chapter'),
    '#required' => TRUE,
    '#default_value' => $chapter_data->name,
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

function edit_chapter_title_form_validate($form, &$form_state)
{
  if (!check_name($form_state['values']['chapter_title']))
    form_set_error('chapter_title', t('Title of the Chapter can contain only alphabets, numbers and spaces.'));
}

function edit_chapter_title_form_submit($form, &$form_state)
{
  global $user;

  /************************ start approve book details ************************/
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('textbook_companion/code');
  }
  if ($proposal_data->proposal_status  != 1)
  {
    switch ($proposal_data->proposal_status )
    {
    case 0:
      drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
      drupal_goto('textbook_companion/code');
      return;
      break;
    case 2:
      drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'textbook_companion/proposal') . '.'), 'error');
      drupal_goto('textbook_companion/code');
      return;
      break;
    case 3:
      drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'textbook_companion/proposal') . '.'), 'status');
      drupal_goto('textbook_companion/code');
      return;
      break;
    default:
       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
       drupal_goto('textbook_companion/code');
       return;
       break;
    }
  }

  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
  $preference_data = db_fetch_object($preference_q);
  if (!$preference_data)
  {
    drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
    drupal_goto('textbook_companion/code');
    return;
  }
  /************************ end approve book details **************************/

  $chapter_id = arg(4);

  $chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d AND preference_id = %d", $chapter_id, $preference_data->id);
  $chapter_data = db_fetch_object($chapter_q);
  if (!$chapter_data)
  {
    drupal_set_message(t('Invalid chapter.'), 'error');
    drupal_goto('textbook_companion/code');
    return;
  }
  db_query("UPDATE {textbook_companion_chapter} SET name = '%s' WHERE id = %d", $form_state['values']['chapter_title'], $chapter_id);
  drupal_set_message(t('Title of the Chapter updated.'), 'status');
}
