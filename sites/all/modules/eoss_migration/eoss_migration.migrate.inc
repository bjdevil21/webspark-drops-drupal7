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
    'source_connection' => 'eossMigrate',
    'source_version'=> '7',
    'format_mappings' => array(
      '1' => 'filtered_html',
    ),
  );

  $submissions_arguments = $common_arguments + array(
      'class_name' => 'EOSSMigrateWebformSubmission',
      'machine_name' => 'EOSSMigrateWebformSubmission',
      'description' => t('Migration of submissions from EOSS.asu.edu'),
      'group_name' => 'EOSSWebform',
      'dependencies' => array('EOSSDrupalUser7Migration'), // After users come in
    );
  $submitted_data_arguments = $common_arguments + array(
      'class_name' => 'EOSSMigrateWebformSubmittedData',
      'machine_name' => 'EOSSMigrateWebformSubmittedData',
      'description' => t('Migration of submitted data from EOSS.asu.edu'),
      'group_name' => 'EOSSWebform',
      'dependencies' => array('EOSSMigrateWebformSubmission'),
    );
  $last_download_arguments = $common_arguments + array(
      'class_name' => 'EOSSWebformLastDownload',
      'machine_name' => 'EOSSWebformLastDownload',
      'description' => t('Migration of last download of submission logs by EOSS.asu.edu webform administrators'),
      'group_name' => 'EOSSSubmissions',
      'dependencies' => array('EOSSMigrateWebformSubmission'),
    );
  $users_arguments = $common_arguments + array(
      'class_name' => 'EOSSDrupalUser7Migration',
      'machine_name' => 'EOSSDrupalUser7Migration',
      'description' => t('Migration of webform submission user IDs'),
      'group_name' => 'EOSSUsers',
//      'soft_dependencies' => array(''), // After features + custom MySQL - which could've been done in Migrate API!!
    );
  $files_arguments = $common_arguments + array(
      'class_name' => 'EOSSSubmissionFiles',
      'machine_name' => 'EOSSSubmissionFiles',
      'description' => t('Migration of webform submission files'),
      'group_name' => 'EOSSFiles',
      'dependencies' => array('EOSSMigrateWebformSubmittedData'),
    );

  $api = array(
    'api' => 2,
    // destination handlers - Magically extends an existing destination and adds functionality
    // that the source may know nothing about.
    // Ex. MigrateCommentNodeHandler - adds comment fields to nodes when imported
    // from a source without any comments (if desired)
    'destination handlers' => array(
 //     'EOSSDrupalDestinationUser',
    ),
    // Field Handlers - Manipulate the data for Drupal consumption -
    'field handlers' => array(
 //     'MigrateNameHandler',
    ),
    'migrations' => array(
      'EOSSMigrateDestinationWebformSubmission' => $submissions_arguments,
//      'EOSSWebformSubmittedData' => $submitted_data_arguments,
//      'EOSSWebformLastDownload' => $last_download_arguments,
      'EOSSDrupalUser7Migration' => $users_arguments,
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