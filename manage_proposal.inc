<?php
// $Id$

function _proposal_pending()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  $pending_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE proposal_status = 0 ORDER BY id DESC");
  while ($pending_data = db_fetch_object($pending_q))
  {
    $pending_rows[$pending_data->id] = array(date('d-m-Y', $pending_data->creation_date), l($pending_data->full_name, 'user/' . $pending_data->uid), date('d-m-Y', $pending_data->completion_date), l('Edit', 'manage_proposal/approve/' . $pending_data->id));
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
    $proposal_rows[] = array(date('d-m-Y', $proposal_data->creation_date), $preference_data->book, $preference_data->author, l($proposal_data->full_name, 'user/' . $proposal_data->uid), date('d-m-Y', $proposal_data->completion_date), $proposal_status, l('Edit', 'manage_proposal/edit/' . $proposal_data->id));
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

function proposal_edit_form($form_state)
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
    '#value' => l(t('Cancel'), 'manage_proposal'),
  );
  return $form;
}

function proposal_edit_form_submit($form, &$form_state)
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


