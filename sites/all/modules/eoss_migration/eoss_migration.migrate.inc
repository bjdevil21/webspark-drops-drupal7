<?php

/**
 * @file
 * this file is automatically loaded when hook_migrate_api is invoked by the
 * Migrate module.
 */

/*
 * Implement hook_migrate_api().
 */
function eoss_migration_migrate_api() {

  // Pre-set common arguments for creating classes
  $common_arguments = array(
    'source_connection' => 'legacy',
    'source_version'=> 'd7',
    'format_mappings' => array(
      '1' => 'filtered_html',
    ),
  );

  $submissions_arguments = $common_arguments + array(
      'class_name' => 'EOSSWebformSubmissions',
      'machine_name' => 'EOSSWebformSubmissions',
      'description' => t('Migration of submissions from EOSS.asu.edu'),
      'group_name' => 'EOSSWebform',
//      'dependencies' => array('WebformEmails'), // Why is this needed?
    );
  $submitted_data_arguments = $common_arguments + array(
      'class_name' => 'EOSSWebformSubmittedData',
      'machine_name' => 'EOSSWebformSubmittedData',
      'description' => t('Migration of submitted data from EOSS.asu.edu'),
      'group_name' => 'EOSSWebform',
      'dependencies' => array('WebformSubmissions'),
    );
  $last_download_arguments = $common_arguments + array(
      'class_name' => 'EOSSWebformLastDownload',
      'machine_name' => 'EOSSWebformLastDownload',
      'description' => t('Migration of last download of submission logs by EOSS.asu.edu webform administrators'),
      'dependencies' => array('WebformSubmittedData'),
      'group_name' => 'EOSSSubmissions',
    );
  $users_arguments = $common_arguments + array(
      'class_name' => 'EOSSWebformSubmissionUser',
      'machine_name' => 'EOSSWebformSubmissionUser',
      'description' => t('Migration of webform submission user IDs'),
      'dependencies' => array('WebformSubmittedData'),
      'group_name' => 'EOSSUsers',
    );
  $files_arguments = $common_arguments + array(
      'class_name' => 'EOSSSubmissionFiles',
      'machine_name' => 'EOSSSubmissionFiles',
      'description' => t('Migration of webform submission files'),
      'dependencies' => array('WebformSubmittedData'),
      'group_name' => 'EOSSFiles',
    );

  $api = array(
    'api' => 2,
    // DH - Magically extends an existing destination and adds functionality
    // that the source may know nothing about.
    // Ex. MigrateCommentNodeHandler - adds comment fields to nodes when imported
    // from a source without any comments (if desired)
    'destination handlers' => array(
      'EOSSDrupalDestinationUser',
    ),
    // FH -
    'field handlers' => array(
      'MigrateNameHandler',
      'MigrateMediaFieldHandler',
    ),
    'migrations' => array(
//      'EOSSWebformSubmissions' => $submissions_arguments,
//      'EOSSWebformSubmittedData' => $submitted_data_arguments,
//      'EOSSWebformLastDownload' => $last_download_arguments,
      'EOSSSubmissionUsers' => $users_arguments,
//      'EOSSSubmissionFiles' => $files_arguments,
    ),
    'groups' => array(
      'EOSSSubmissions' => array(
        'title' => t('EOSS Webform Submissions'),
      ),
      'EOSSUsers' => array(
        'title' => t('EOSS Webform Submission Users'),
      ),
      'EOSSFiles' => array(
        'title' => t('EOSS Webform Submitted Files'),
      )
    ),
  );

  return $api;
}