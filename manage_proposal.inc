<?php
// $Id$

function _proposal_pending()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  $pending_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 ORDER BY id DESC");
  while ($pending_data = db_fetch_object($pending_q))
  {
    $pending_rows[$pending_data->id] = array(date('d-m-Y', $pending_data->creation_date), l($pending_data->full_name, 'user/' . $pending_data->uid), date('d-m-Y', $pending_data->completion_date), l('Approve', 'manage_proposal/approve/' . $pending_data->id) . ' | ' . l('Edit', 'manage_proposal/edit/' . $pending_data->id));
  }

  /* check if there are any pending proposals */
  if (!$pending_rows)
  {
    drupal_set_message(t('There are no pending proposals.'), 'status');
    return '';
  }

  $pending_header = array('Date of Submission', 'Contributor Name', 'Date of Completion', 'Action');
  $output = theme_table($pending_header, $pending_rows);
  return $output; 
}

function _proposal_all()
{
  /* get pending proposals to be approved */
  $proposal_rows = array();
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} ORDER BY id DESC");
  while ($proposal_data = db_fetch_object($proposal_q))
  {
    /* get preference */
    $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $preference_data = db_fetch_object($preference_q);

    $proposal_status = '';
    switch ($proposal_data->proposal_status)
    {
    case 0: $proposal_status = 'Pending'; break;
    case 1: $proposal_status = 'Approved'; break;
    case 2: $proposal_status = 'Dis-approved'; break;
    case 3: $proposal_status = 'Completed'; break;
    default: $proposal_status = 'Unknown'; break;
    }
    $proposal_rows[] = array(date('d-m-Y', $proposal_data->creation_date), $preference_data->book, $preference_data->author, l($proposal_data->full_name, 'user/' . $proposal_data->uid), date('d-m-Y', $proposal_data->completion_date), $proposal_status, l('Status', 'manage_proposal/status/' . $proposal_data->id) . ' | ' . l('Edit', 'manage_proposal/edit/' . $proposal_data->id));
  }

  /* check if there are any pending proposals */
  if (!$proposal_rows)
  {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  }

  $proposal_header = array('Date of Submission', 'Title of the Book', 'Author', 'Contributor Name', 'Expected Date of Completion', 'Status');
  $output = theme_table($proposal_header, $proposal_rows);
  return $output; 
}

/******************************************************************************/
/************************** PROPOSAL APPROVAL FORM ****************************/
/******************************************************************************/

function proposal_approval_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = arg(2);
  $result = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 and id = %d", $proposal_id);
  if ($result)
  {
    if ($row = db_fetch_object($result))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }

  $form['full_name'] = array(
    '#type' => 'item',
    '#value' => l($row->full_name, 'user/' . $row->uid),
    '#title' => t('Contributor Name'),
  );
  $form['email'] = array(
    '#type' => 'item',
    '#value' => user_load($row->uid)->mail,
    '#title' => t('Email'),
  );
  $form['mobile'] = array(
    '#type' => 'item',
    '#value' => $row->mobile,
    '#title' => t('Mobile'),
  );
  $form['how_project'] = array(
    '#type' => 'item',
    '#value' => $row->how_project,
    '#title' => t('How did you come to know about this project'),
  );
  $form['course'] = array(
    '#type' => 'item',
    '#value' => $row->course,
    '#title' => t('Course'),
  );
  $form['branch'] = array(
    '#type' => 'item',
    '#value' => $row->branch,
    '#title' => t('Department/Branch'),
  );
  $form['university'] = array(
    '#type' => 'item',
    '#value' => $row->university,
    '#title' => t('University/Institute'),
  );
  $form['faculty'] = array(
    '#type' => 'item',
    '#value' => $row->faculty,
    '#title' => t('College Teacher/Professor'),
  );
  $form['reviewer'] = array(
    '#type' => 'item',
    '#value' => $row->reviewer,
    '#title' => t('Reviewer'),
  );
  $form['completion_date'] = array(
    '#type' => 'item',
    '#value' => date('d-m-Y', $row->completion_date),
    '#title' => t('Expected Date of Completion'),
  );

  /* get book preference */
  $preference_rows = array();
  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d ORDER BY pref_number ASC", $proposal_id);
  while ($preference_data = db_fetch_object($preference_q))
  {
    $preference_rows[$preference_data->id] = $preference_data->book . ' (Written by ' . $preference_data->author . ')';
  }

  $form['book_preference'] = array(
      '#type' => 'radios',
      '#title' => t('Book Preferences'),
      '#options' => $preference_rows,
  );

  $form['disapprove'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disapprove all the above book preferences'),
  );

  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Reason for disapproval'),
  );

  $form['proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal'),
  );
  return $form;
}

function proposal_approval_form_submit($form, &$form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = $form_state['values']['proposal_id'];
  $result = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 and id = %d", $proposal_id);
  if ($result)
  {
    if ($row = db_fetch_object($result))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }

  /* disapprove */
  if ($form_state['values']['disapprove'])
  {
    db_query("UPDATE {textbook_companion_proposal} SET approver_uid = %d, approval_date = %d, proposal_status = 2, message = '%s' WHERE id = %d", $user->uid, time(), $form_state['values']['message'], $proposal_id);
    db_query("UPDATE {textbook_companion_preference} SET approval_status = 2 WHERE proposal_id = %d", $proposal_id);

    /* sending email */
    $book_user = user_load($row->uid);
    $param['proposal_disapproved']['proposal_id'] = $proposal_id;
    $param['proposal_disapproved']['user_id'] = $row->uid;
    $email_to = $book_user->mail . ', ' . variable_get('textbook_companion_emails', '');
    if (!drupal_mail('textbook_companion', 'proposal_disapproved', $email_to , language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    drupal_set_message('Book proposal dis-approved. User has been notified of the dis-approval.', 'error');
    drupal_goto('manage_proposal');
    return;
  }

  /* get book preference and set the status */
  $preference_id = $form_state['values']['book_preference'];
  db_query("UPDATE {textbook_companion_proposal} SET approver_uid = %d, approval_date = %d, proposal_status = 1 WHERE id = %d", $user->uid, time(), $proposal_id);
  db_query("UPDATE {textbook_companion_preference} SET approval_status = 1 WHERE id = %d", $preference_id);

  /* sending email */
  $book_user = user_load($row->uid);
  $param['proposal_approved']['proposal_id'] = $proposal_id;
  $param['proposal_approved']['user_id'] = $row->uid;
  $email_to = $book_user->mail . ', ' . variable_get('textbook_companion_emails', '');
  if (!drupal_mail('textbook_companion', 'proposal_approved', $email_to , language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');

  drupal_set_message('Book proposal approved. User has been notified of the approval', 'status');
  drupal_goto('manage_proposal');
  return;
}


/******************************************************************************/
/*************************** PROPOSAL STATUS FORM *****************************/
/******************************************************************************/

function proposal_status_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = arg(2);
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $proposal_id);
  if (!$proposal_data = db_fetch_object($proposal_q))
  {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }

  $form['full_name'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->full_name,
    '#title' => t('Contributor Name'),
  );
  $form['email'] = array(
    '#type' => 'item',
    '#value' => user_load($proposal_data->uid)->mail,
    '#title' => t('Email'),
  );
  $form['mobile'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->mobile,
    '#title' => t('Mobile'),
  );
  $form['how_project'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->how_project,
    '#title' => t('How did you come to know about this project'),
  );
  $form['course'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->course,
    '#title' => t('Course'),
  );
  $form['branch'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->branch,
    '#title' => t('Department/Branch'),
  );
  $form['university'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->university,
    '#title' => t('University/Institute'),
  );
  $form['faculty'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->faculty,
    '#title' => t('College Teacher/Professor'),
  );
  $form['reviewer'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->reviewer,
    '#title' => t('Reviewer'),
  );
  $form['completion_date'] = array(
    '#type' => 'item',
    '#value' => date('d-m-Y', $proposal_data->completion_date),
    '#title' => t('Expected Date of Completion'),
  );

  /* get book preference */
  $preference_html = '<ul>';
  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d ORDER BY pref_number ASC", $proposal_id);
  while ($preference_data = db_fetch_object($preference_q))
  {
    if ($preference_data->approval_status == 1)
      $preference_html .= '<li><strong>' . $preference_data->book . ' (Written by ' . $preference_data->author . ')  - Approved Book</strong></li>';
    else
      $preference_html .= '<li>' . $preference_data->book . ' (Written by ' . $preference_data->author . ')</li>';
  }
  $preference_html .= '</ul>';

  $form['book_preference'] = array(
      '#type' => 'item',
      '#value' => $preference_html,
      '#title' => t('Book Preferences'),
  );

  $proposal_status = '';
  switch ($proposal_data->proposal_status)
  {
  case 0: $proposal_status = t('Pending'); break;
  case 1: $proposal_status = t('Approved'); break;
  case 2: $proposal_status = t('Dis-approved'); break;
  case 3: $proposal_status = t('Completed'); break;
  default: $proposal_status = t('Unkown'); break;
  }
  $form['proposal_status'] = array(
      '#type' => 'item',
      '#value' => $proposal_status,
      '#title' => t('Proposal Status'),
  );

  if ($proposal_data->proposal_status == 2) {
    $form['message'] = array(
      '#type' => 'item',
      '#value' => $proposal_data->message,
      '#title' => t('Reason for disapproval'),
    );
  }

  if ($proposal_data->proposal_status == 1)
  {
    $form['completed'] = array(
      '#type' => 'checkbox',
      '#title' => t('Completed'),
      '#description' => t('Check if user has completed all the book examples.'),
    );
  }

  if ($proposal_data->proposal_status == 0)
  {
    $form['approve'] = array(
      '#type' => 'item',
      '#value' => l('Click here', 'manage_proposal/approve/' . $proposal_id),
      '#title' => t('Approve'),
    );
  }

  $form['proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal/all'),
  );
  return $form;
}

function proposal_status_form_submit($form, &$form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = $form_state['values']['proposal_id'];
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $proposal_id);
  if (!$proposal_data = db_fetch_object($proposal_q))
  {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }

  /* set the book status to completed */
  if ($form_state['values']['completed'] == 1)
  {
    db_query("UPDATE {textbook_companion_proposal} SET proposal_status = 3 WHERE id = %d", $proposal_id);

    /* sending email */
    $book_user = user_load($row->uid);
    $param['proposal_approved']['proposal_id'] = $proposal_id;
    $param['proposal_approved']['user_id'] = $row->uid;
    $email_to = $book_user->mail . ', ' . variable_get('textbook_companion_emails', '');
    if (!drupal_mail('textbook_companion', 'proposal_completed', $email_to , language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');
  
    drupal_set_message('Congratulations! Book proposal has been marked as completed. User has been notified of the completion.', 'status');
  }
  drupal_goto('manage_proposal');
  return;
}


/******************************************************************************/
/**************************** PROPOSAL EDIT FORM ******************************/
/******************************************************************************/

function proposal_edit_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = arg(2);
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    $proposal_data = db_fetch_object($proposal_q);
    if (!$proposal_data)
    {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }

  $preference1_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 1);
  $preference1_data = db_fetch_object($preference1_q);
  $preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 2);
  $preference2_data = db_fetch_object($preference2_q);
  $preference3_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 3);
  $preference3_data = db_fetch_object($preference3_q);

  $form['full_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Full Name'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $proposal_data->full_name,
  );
  $form['email_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#size' => 30,
    '#maxlength' => 64,
    '#value' => $user->mail,
    '#disabled' => TRUE,
  );
  $form['mobile'] = array(
    '#type' => 'textfield',
    '#title' => t('Mobile No.'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $proposal_data->mobile,
  );
  $form['how_project'] = array(
    '#type' => 'select',
    '#title' => t('How did you come to know about this project'),
    '#options' => array('Scilab Website' => 'Scilab Website',
                        'Friend' => 'Friend',
                        'Professor/Teacher' => 'Professor/Teacher',
                        'Mailing List' => 'Mailing List',
                        'Poster in my/other college' => 'Poster in my/other college',
                        'Others' => 'Others'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->how_project,
  );
  $form['course'] = array(
    '#type' => 'textfield',
    '#title' => t('Course'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $proposal_data->course,
  );
  $form['branch'] = array(
    '#type' => 'select',
    '#title' => t('Department/Branch'),
    '#options' => array('Electrical Engineering' => 'Electrical Engineering',
                        'Electronics Engineering' => 'Electronics Engineering',
                        'Computer Engineering' => 'Computer Engineering',
                        'Chemical Engineering' => 'Chemical Engineering',
                        'Instrumentation Engineering' => 'Instrumentation Engineering',
                        'Mechanical Engineering' => 'Mechanical Engineering',
                        'Civil Engineering' => 'Civil Engineering',
                        'Physics' => 'Physics',
                        'Mathematics' => 'Mathematics',
                        'Others' => 'Others'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->branch,
  );
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $proposal_data->university,
  );
  $form['faculty'] = array(
    '#type' => 'textfield',
    '#title' => t('College Teacher/Professor'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $proposal_data->faculty,
  );
  $form['reviewer'] = array(
    '#type' => 'textfield',
    '#title' => t('Reviewer'),
    '#size' => 30,
    '#maxlength' => 64,
    '#default_value' => $proposal_data->reviewer,
  );
  $form['completion_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Expected Date of Completion'),
    '#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
    '#size' => 10,
    '#maxlength' => 10,
    '#default_value' => date('d-m-Y', $proposal_data->completion_date),
  );
  $form['preference1'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 1'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference1']['book1'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference1_data->book,
  );
  $form['preference1']['author1'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference1_data->author,
  );
  $form['preference1']['isbn1'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference1_data->isbn,
  );
  $form['preference1']['publisher1'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference1_data->publisher,
  );
  $form['preference1']['edition1'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference1_data->edition,
  );
  $form['preference1']['year1'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference1_data->year,
  );
  $form['preference2'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 2'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference2']['book2'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference2_data->book,
  );
  $form['preference2']['author2'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference2_data->author,
  );
  $form['preference2']['isbn2'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference2_data->isbn,
  );
  $form['preference2']['publisher2'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference2_data->publisher,
  );
  $form['preference2']['edition2'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference2_data->edition,
  );
  $form['preference2']['year2'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference2_data->year,
  );
  $form['preference3'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 3'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference3']['book3'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference3_data->book,
  );
  $form['preference3']['author3'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference3_data->author,
  );
  $form['preference3']['isbn3'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference3_data->isbn,
  );
  $form['preference3']['publisher3'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference3_data->publisher,
  );
  $form['preference3']['edition3'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 30,
    '#maxlength' => 64,
    '#required' => TRUE,
    '#default_value' => $preference3_data->edition,
  );
  $form['preference3']['year3'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference3_data->year,
  );

  /* hidden fields */
  $form['hidden_proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal'),
  );
  return $form;
}

function proposal_edit_form_validate($form, &$form_state)
{
  /* mobile */
  if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['mobile']))
    form_set_error('mobile', t('Invalid mobile number'));

  /* date of completion */
  if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['completion_date']))
    form_set_error('completion_date', t('Invalid expected date of completion'));

  list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
  $d = (int)$d; $m = (int)$m; $y = (int)$y;
  if (!checkdate($m, $d, $y))
    form_set_error('completion_date', t('Invalid expected date of completion'));
  if (mktime(0, 0, 0, $m, $d, $y) <= time())
  	form_set_error('completion_date', t('Expected date of completion should be in future'));  

  /* edition */
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition1']))
    form_set_error('edition1', t('Invalid edition for Book Preference 1'));
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition2']))
    form_set_error('edition2', t('Invalid edition for Book Preference 2'));
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition3']))
    form_set_error('edition3', t('Invalid edition for Book Preference 3'));

  /* year of publication */
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year1']))
    form_set_error('year1', t('Invalid year of pulication for Book Preference 1'));
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year2']))
    form_set_error('year2', t('Invalid year of pulication for Book Preference 2'));
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year3']))
    form_set_error('year3', t('Invalid year of pulication for Book Preference 3'));

  /* year of publication */
  $cur_year = date('Y');
  if ((int)$form_state['values']['year1'] > $cur_year)
    form_set_error('year1', t('Year of pulication should be not in the future for Book Preference 1'));
  if ((int)$form_state['values']['year2'] > $cur_year)
    form_set_error('year2', t('Year of pulication should be not in the future for Book Preference 2'));
  if ((int)$form_state['values']['year3'] > $cur_year)
    form_set_error('year3', t('Year of pulication should be not in the future for Book Preference 3'));
  
  /* isbn */
  if (!preg_match('/^[0-9\-]+$/', $form_state['values']['isbn1']))
    form_set_error('isbn1', t('Invalid ISBN for Book Preference 1'));
  if (!preg_match('/^[0-9\-]+$/', $form_state['values']['isbn2']))
    form_set_error('isbn2', t('Invalid ISBN for Book Preference 2'));
  if (!preg_match('/^[0-9\-]+$/', $form_state['values']['isbn3']))
    form_set_error('isbn3', t('Invalid ISBN for Book Preference 3'));
  return;
}

function proposal_edit_form_submit($form, &$form_state)
{
  /* completion date to timestamp */
  list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
  $completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);

  $proposal_id = $form_state['values']['hidden_proposal_id'];

  db_query("UPDATE {textbook_companion_proposal} SET full_name = '%s', mobile = '%s', how_project = '%s', course = '%s', branch = '%s', university = '%s', faculty = '%s', reviewer = '%s', completion_date = %d WHERE id = %d",
    $form_state['values']['full_name'],
    $form_state['values']['mobile'],
    $form_state['values']['how_project'],
    $form_state['values']['course'],
    $form_state['values']['branch'],
    $form_state['values']['university'],
    $form_state['values']['faculty'],
    $form_state['values']['reviewer'],
    $completion_date_timestamp,
    $proposal_id);

  $preference1_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 1);
  $preference1_data = db_fetch_object($preference1_q);
  $preference1_id = $preference1_data->id;
  $preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 2);
  $preference2_data = db_fetch_object($preference2_q);
  $preference2_id = $preference2_data->id;
  $preference3_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 3);
  $preference3_data = db_fetch_object($preference3_q);
  $preference3_id = $preference3_data->id;

  db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
    $form_state['values']['book1'],
    $form_state['values']['author1'],
    $form_state['values']['isbn1'],
    $form_state['values']['publisher1'],
    $form_state['values']['edition1'],
    $form_state['values']['year1'],
    $preference1_id);
  db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
    $form_state['values']['book2'],
    $form_state['values']['author2'],
    $form_state['values']['isbn2'],
    $form_state['values']['publisher2'],
    $form_state['values']['edition2'],
    $form_state['values']['year2'],
    $preference2_id);
  db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
    $form_state['values']['book3'],
    $form_state['values']['author3'],
    $form_state['values']['isbn3'],
    $form_state['values']['publisher3'],
    $form_state['values']['edition3'],
    $form_state['values']['year3'],
    $preference3_id);
  drupal_set_message(t('Proposal Updated'), 'status');
}

