<?php
// $Id$

function code_approval()
{
  /* get a list of unapproved chapters */
  $pending_chapter_q = db_query("SELECT c.id as c_id, c.number as c_number, c.name as c_name, c.preference_id as c_preference_id FROM {textbook_companion_example} as e JOIN {textbook_companion_chapter} as c ON c.id = e.chapter_id  WHERE e.approval_status = 0");
  if (!$pending_chapter_q)
  {
    drupal_set_message(t('There are no pending code approvals.'), 'status');
    return '';
  }
  $rows = array();
  while ($row = db_fetch_object($pending_chapter_q))
  {
    /* get preference data */
    $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $row->c_preference_id);
    $preference_data = db_fetch_object($preference_q);
    /* get proposal data */
    $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
    $proposal_data = db_fetch_object($proposal_q);
    /* setting table row information */
    $rows[] = array($preference_data->book, $row->c_number, $row->c_name, $proposal_data->full_name, l('Edit', 'code_approval/approve/' . $row->c_id));
  }

  /* check if there are any pending proposals */                                   
  if (!$rows)
  {
    drupal_set_message(t('There are no pending proposals'), 'status');
    return '';
  }

  $header = array('Title of the Book', 'Chapter Number', 'Title of the Chapter', 'Contributor Name', 'Actions');
  $output = theme_table($header, $rows);
  return $output; 
}

function code_approval_form($form_state)
{
  /* get a list of unapproved chapters */
  $chapter_id = arg(2);
  $pending_chapter_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d", $chapter_id);
  if ($pending_chapter_data = db_fetch_object($pending_chapter_q))
  {
    /* get preference data */
    $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $pending_chapter_data->preference_id);
    $preference_data = db_fetch_object($preference_q);
    /* get proposal data */
    $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
    $proposal_data = db_fetch_object($proposal_q);
  } else {
    drupal_set_message(t('Invalid chapter selected.'), 'error');
    drupal_goto('code_approval');
    return;
  }

  $form['#tree'] = TRUE;

  $form['contributor'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->full_name,
    '#title' => t('Contributor Name'),
  );

  $form['book_details']['book'] = array(
    '#type' => 'item',
    '#value' => $preference_data->book,
    '#title' => t('Title of the Book'),
  );

  $form['book_details']['number'] = array(
    '#type' => 'item',
    '#value' => $pending_chapter_data->number,
    '#title' => t('Chapter Number'),
  );

  $form['book_details']['name'] = array(
    '#type' => 'item',
    '#value' => $pending_chapter_data->name,
    '#title' => t('Title of the Chapter'),
  );

  /* get example data */
  $example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d", $chapter_id);
  while ($example_data = db_fetch_object($example_q))
  {
    $form['example_details'][$example_data->id] = array(
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#collapsed' => TRUE,
    );
    $form['example_details'][$example_data->id]['example_number'] = array(
      '#type' => 'item',
      '#value' => $example_data->number,
      '#title' => t('Example Number'),
    );
  
    $form['example_details'][$example_data->id]['example_caption'] = array(
      '#type' => 'item',
      '#value' => $example_data->caption,
      '#title' => t('Example Caption'),
    );

    $form['example_details'][$example_data->id]['download'] = array(
      '#type' => 'markup',
      '#value' => l('Download Example', 'download/example/' . $example_data->id),
    );

    $form['example_details'][$example_data->id]['approved'] = array(
      '#type' => 'radios',
      '#options' => array('Approved', 'Dis-approved'),
    );

    $form['example_details'][$example_data->id]['message'] = array(
      '#type' => 'textfield',
      '#title' => t('Reason for dis-approval'),
    );

    $form['example_details'][$example_data->id]['example_id'] = array(
      '#type' => 'hidden',
      '#value' => $example_data->id,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}

function code_approval_form_submit($form, &$form_state)
{
  global $user;

  foreach ($form_state['values']['example_details'] as $ex_id => $ex_data)
  {
    $example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE id = %d LIMIT 1", $ex_data['example_id']);
    $example_data = db_fetch_object($example_q);
    $chapter_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_chapter} WHERE id = %d LIMIT 1", $example_data->chapter_id));
    $preference_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d LIMIT 1", $chapter_data->preference_id));
    $proposal_data = db_fetch_object(db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d LIMIT 1", $preference_data->proposal_id));
    $user_data = user_load($proposal_data->uid);

    if ($ex_data['approved'] == "0")
    {
      db_query("UPDATE {textbook_companion_example} SET approval_status = 1, approver_uid = %d, approval_date = %d WHERE id = %d", $user->uid, time(), $ex_data['example_id']);

      /* sending email */
      $email_to = $user_data->mail . ', ' . variable_get('textbook_companion_emails', '');
      $param['example_approved']['example_id'] = $ex_data['example_id'];
      $param['example_approved']['user_id'] = $user_data->uid;
      if (!drupal_mail('textbook_companion', 'example_approved', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
        drupal_set_message('Error sending email message.', 'error');

    } else if ($ex_data['approved'] == "1") {
      db_query("UPDATE {textbook_companion_example} SET approval_status = 2, approver_uid = %d, approval_date = %d WHERE id = %d", $user->uid, time(), $ex_data['example_id']);

      /* sending email */
      $email_to = $user_data->mail . ', ' . variable_get('textbook_companion_emails', '');
      $param['example_disapproved']['example_id'] = $ex_data['example_id'];
      $param['example_disapproved']['user_id'] = $user_data->uid;
      $param['example_disapproved']['message'] = $ex_data['message'];
      if (!drupal_mail('textbook_companion', 'example_disapproved', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
        drupal_set_message('Error sending email message.', 'error');
    }
  }
  drupal_set_message('Approval status updated.', 'status');
  drupal_goto('code_approval');
}
