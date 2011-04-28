<?php
// $Id$

function textbook_companion_settings_form($form_state)
{
  $form['emails'] = array(
    '#type' => 'textfield',
    '#title' => t('Notification emails'),
    '#description' => t('A comma separated list of email addresses to receive notifications emails'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_emails', ''),
  );

  $form['from_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Outgoing from email address'),
    '#description' => t('Email address to be display in the from field of all outgoing messages'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_from_email', ''),
  );

  $form['extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Allow upload of following file extensions'),
    '#description' => t('A comma separated list of file extensions that are permitted to be uploaded on the server'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('textbook_companion_extensions', ''),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}

function textbook_companion_settings_form_validate($form, &$form_state)
{
  return;
}

function textbook_companion_settings_form_submit($form, &$form_state)
{
  variable_set('textbook_companion_emails', $form_state['values']['emails']);
  variable_set('textbook_companion_from_email', $form_state['values']['from_email']);
  variable_set('textbook_companion_extensions', $form_state['values']['extensions']);
  drupal_set_message(t('Settings updated'), 'status');
}
