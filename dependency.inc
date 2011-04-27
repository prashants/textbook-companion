<?php
// $Id$

function upload_dependency_form($form_state)
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

  $form['existing_depfile'] = array(
    '#type' => 'item',
    '#value' => _list_existing_dependency($row->pre_id),
    '#title' => t('List of existing dependency files for this book'),
  );

  $form['depfile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload Dependency Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['depfile']['depfile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file'),
      '#description' => t("Allowed file extensions : ") . variable_get('textbook_companion_extensions', ''),
  );
  $form['depfile']['depfile1_caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption for dependency file'),
      '#size' => 15,
  );
  $form['depfile']['depfile1_description'] = array(
      '#type' => 'textarea',
      '#title' => t('Brief Description of the dependency file'),
  );
/*
  $form['depfile']['depfile2_caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption for dependency file 2'),
      '#size' => 15,
  );
  $form['depfile']['depfile2'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file 2'),
      '#size' => 48,
      '#description' => t("Allowed file extensions : ") . variable_get('textbook_companion_extensions', ''),
  );
  $form['depfile']['depfile2_description'] = array(
      '#type' => 'textarea',
      '#title' => t('Brief Description of the Dependency file 2'),
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
      '#description' => t("Allowed file extensions : ") . variable_get('textbook_companion_extensions', ''),
  );
  $form['depfile']['depfile3_description'] = array(
      '#type' => 'textarea',
      '#title' => t('Brief Description of the Dependency file 3'),
  );
*/
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Back'), 'textbook_companion/code/upload'),
  );
  return $form;
}

function upload_dependency_form_validate($form, &$form_state)
{
  global $user;

  /* get approved book details */
  $book_q = db_query("SELECT pro.id as pro_id, pre.id as pre_id, pre.book as pre_book, pro.full_name as pro_full_name FROM {textbook_companion_proposal} pro JOIN {textbook_companion_preference} pre ON pro.id = pre.proposal_id WHERE pro.proposal_status = 1 AND pre.approval_status = 1 AND pro.uid = %d", $user->uid);
  $book_data = db_fetch_object($book_q);

  if (isset($_FILES['files']))
  {
    /* check for valid filename extensions */  
    $allowed_extensions = explode(',' , variable_get('textbook_companion_extensions', ''));
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
    {
      if ($file_name)
      {
         $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
         if (!in_array($temp_extension, $allowed_extensions))
           form_set_error($file_form_name, t('Only ' . variable_get('textbook_companion_extensions', '') . ' extensions can be uploaded.'));
         if ($_FILES['files']['size'][$file_form_name] <= 0)
           form_set_error($file_form_name, t('File size cannot be zero.'));

         /* check if file already exists */
         $dep_exists_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_dependency_files} WHERE filename = '%s' AND preference_id = %d", $_FILES['files']['name'][$file_form_name], $book_data->pre_id));
         if ($dep_exists_data)
           form_set_error($file_form_name, t('Dependency file for this book has been already uploaded. Please rename the file and try again.'));

         /* check if valid file name */
         if (!textbook_companion_check_valid_filename($_FILES['files']['name'][$file_form_name]))
           form_set_error($file_form_name, t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));
      }
    }
  }
}

function upload_dependency_form_submit($form, &$form_state) {
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

  $dest_path .= 'DEPENDENCIES' . '/';
  if (!is_dir($base_path . $dest_path))
    mkdir($base_path . $dest_path);

  /* uploading dependencies */
  $file_upload_counter = 0;
  $dependency_ids = array();
  foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
  {
    if ($file_name)
    {
      /* uploading file */
      if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $base_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        /* for uploaded files making an entry in the database */
        db_query("INSERT INTO {textbook_companion_dependency_files} (preference_id, filename, filepath, filemime, filesize, caption, description, timestamp)
          VALUES (%d, '%s', '%s', '%s', %d, '%s', '%s', %d)",
          $preference_id,
          $_FILES['files']['name'][$file_form_name],
          $dest_path . $_FILES['files']['name'][$file_form_name],
          $_FILES['files']['type'][$file_form_name],
          $_FILES['files']['size'][$file_form_name],
          $form_state['values'][$file_form_name . '_caption'],
          $form_state['values'][$file_form_name . '_description'],
          time()
          );
        drupal_set_message($file_name . ' uploaded successfully.', 'status');
        $dependency_ids[] = db_last_insert_id('textbook_companion_dependency_files', 'id');
        $file_upload_counter++;
      } else {
        drupal_set_message('Error uploading dependency : ' . $dest_path . '/' . $_FILES['files']['name'][$file_form_name], 'error');
      }
    }
  }

  if ($file_upload_counter > 0)
  {
    drupal_set_message('Dependencies uploaded successfully.', 'status');
  
    /* sending email */
    $email_to = $user->mail . ', ' . variable_get('textbook_companion_emails', '');
    $param['dependency_uploaded']['dependency_ids'] = $dependency_ids;
    $param['dependency_uploaded']['user_id'] = $user->uid;
    if (!drupal_mail('textbook_companion', 'dependency_uploaded', $email_to, language_default(), $param, NULL, TRUE))
      drupal_set_message('Error sending email message.', 'error');
  }

  drupal_goto('textbook_companion/code/upload_dep');
}


function _list_existing_dependency($book_id)
{
  $return_html = '<ul>';
  $book_dependency_files_q = db_query("SELECT * FROM {textbook_companion_dependency_files} WHERE preference_id = %d ORDER BY filename ASC", $book_id);

  $counter = 0;
  while ($book_dependency_files_data = db_fetch_object($book_dependency_files_q))
  {
    $temp_caption = '';
    if ($book_dependency_files_data->caption)
      $temp_caption = ' (' . $book_dependency_files_data->caption . ')';
    $return_html .= '<li>' . l($book_dependency_files_data->filename . $temp_caption, 'download/dependency/' . $book_dependency_files_data->id) . '</li>';
    $counter++;
  }
  if ($counter == 0)
    $return_html .= '<li>(None)</li>';
  $return_html .= '</ul>';
  return $return_html;
}

