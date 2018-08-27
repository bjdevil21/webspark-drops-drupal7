<?php
/**
 * @file
 * Admin functions for the file_upload_security module.
 */

/**
 * Implements hook_form().
 */
function file_upload_security_admin_form() {
  $private = variable_get('file_private_path', NULL);
  if ($private) {
    $options = array(
      FILE_UPLOAD_SECURITY_WARN => t('Warn of insecure settings'),
      FILE_UPLOAD_SECURITY_PROTECT_NO_WARN => t('Prevent insecure settings'),
      FILE_UPLOAD_SECURITY_PROTECT => t('Prevent insecure settings and show a warning'),
    );

    $form = array(
      'file_upload_security_fix' => array(
        '#type' => 'checkbox',
        '#title' => t('Fix legacy file uploads'),
        '#description' => t('Selecting this option will update all existing file upload fields to store data in the private area, and moves existing files.'),
        '#default_value' => 0,
      ),
    );
  }
  else {
    $options = array(
      FILE_UPLOAD_SECURITY_WARN => t('Warn of insecure settings'),
    );

    drupal_set_message(t('You do not have a private file system configured and so you may only warn users their settings are insecure. Please set up a private file path at !url.', array(
      '!url' => l(t('the file system admin page'), '/admin/config/media/file-system'),
    )));

    $form = array();
  }

  $description = (module_exists('webform')) ? t('Set the level at which the module will function: add a warning message, or prevent insecure settings. Please note: preventing his will automatically store all webform file uploads in the private folders.') : t('Set the level at which the module will function: add a warning message, or prevent insecure settings.');

  $form['file_upload_security_level'] = array(
    '#type' => 'select',
    '#title' => t('Security Level'),
    '#description' => $description,
    '#default_value' => variable_get('file_upload_security_level', NULL),
    '#options' => $options,
  );

  $form = system_settings_form($form);

  if ($private) {
    $form['#submit'][] = 'file_upload_security_admin_form_submit';
  }

  return $form;
}

/**
 * Implements hook_form_submit().
 */
function file_upload_security_admin_form_submit(&$form, &$form_state) {
  if (array_key_exists('values', $form_state) && array_key_exists('file_upload_security_fix', $form_state['values'])) {
    if ($form_state['values']['file_upload_security_fix']) {
      if (array_key_exists('file_upload_security_level', $form_state['values'])) {

        // Batch processing prevents admin form from submitting.
        variable_set('file_upload_security_level', $form_state['values']['file_upload_security_level']);
      }
      file_upload_security_fix_files();
    }
  }
}
