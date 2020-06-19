<?php

/**
 * @file
 * File Upload Security module drush integration.
 */

/**
 * Implements hook_drush_command().
 */
function file_upload_security_drush_command() {
  $items = array();

  $items['file-upload-security-fix-files'] = array(
    'description' => "Update all file upload fields available to anonymous users to have secure settings",
    'options' => array(),
    'drupal dependencies' => array('file_upload_security'),
    'aliases' => array('fus-fix'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function file_upload_security_drush_help($section) {
  switch ($section) {
    case 'drush:file-upload-security-fix-files':
      return dt("Update file field types to store uploaded files in the private storage system, and move all affected uploaded files.");
    break;

  }
}

/**
 * Update field settings and move files to the private scheme.
 */
function drush_file_upload_security_fix_files() {
  if (!variable_get('file_private_path', NULL)) {
    return drush_set_error('PRIVATE_FILE_PATH_NOT_SET', dt('The Drupal Private File Path is not set. You cannot move files without configuring a private path.'));
  }

  drush_print(dt('This action will make changes directly in the database and move files. You should ensure you have a database and file back up before proceeding.'));
  if (!drush_confirm(dt('Do you really want to continue?'))) {
    drush_die('Aborting.');
  }

  file_upload_security_fix_files_drush();

  drush_print(dt('The action has been completed. If you see no errors, then all publicly accessible fields and all webforms have been secured.'));
}
