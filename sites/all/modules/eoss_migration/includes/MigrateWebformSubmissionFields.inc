<?php

/**
 * Helper class for webform submission migration
 */

class MigrateWebformSubmissionFields {

  public function __construct() {
  }

  /**
   * @param string $what_columns
   * @return array of webform_submission DB table columns
   */
  public static function whichWebformDBCols($what_columns = 'all') {
    // Used cols instead of fields to avoid method name overlapping.
    $simple_cols = array(
      'sid' => t('The unique identifier for this submission.'),
      'is_draft' => t('Is this submission not completed?'),
      'submitted' => t('Timestamp of when the form was submitted.'),
      'remote_addr' => t('The IP address of the user that submitted the form.'),
      // EOSS additions
      'uuid' => t('The unique webform submission UUID'),
      'serial' => t('The serial ID of the submission; like a primary key'),
      'completed' => t('Timestamp of when submission ID was completed.'),
      'highest_valid_page' => t(''),
      'modified' => t('Timestamp of when submission ID was last modified.'),
    );
    $complex_cols = array(
      'uid' => t('The id of the user that completed this submission.'),
      'nid' => t('The node ID of the webform that created the submission.'),
    );
    if ($what_columns == 'simple') {
      return array_keys($simple_cols);
    }
    else {
      return array_merge($simple_cols, $complex_cols);
    }
  }
}