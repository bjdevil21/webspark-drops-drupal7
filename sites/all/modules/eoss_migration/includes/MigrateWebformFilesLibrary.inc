<?php

/**
 * Creates directory information for file moves/migrations
 */
class MigrateWebformFilesLibrary {

  /**
   * Directories for file migration (src, dest)
   *
   * @var (bool) $is_private - If TRUE, return default files
   */
  private static $srcAbsFileFolder = '/home/bryan/eoss-migrate/sites/default/files/';
  private static $dstAbsFileFolder = DRUPAL_ROOT . '/sites/default/files/';
  private static $privateFolder = "private/webform/";
  private static $publicFolder = "webform/";

  /**
   * @return string
   */
  public static function getSrcAbsFileFolder(): string {
    return self::$srcAbsFileFolder;
  }
  /**
   * @return string
   */
  public static function getDstAbsFileFolder(): string {
    return self::$dstAbsFileFolder;
  }
  /**
   * @return string
   */
  public static function getPrivateFolder(): string {
    return self::$privateFolder;
  }
  /**
   * @return string
   */
  public static function getPublicFolder(): string {
    return self::$publicFolder;
  }

  // @TODO call variable_get('file_public_path'), etc.

  /**
   * @var (obj upon instantiation) List of Node UUID-based directory names
   * (Does not include the entire file path)
   */
  protected $directoryNames;

  public function getDirectoryNames() {
    return $this->directoryNames;
  }

  /**
   * MigrateWebformFileDirectories constructor. Builds default directory names list upon each instantiation.
   * @param $node_id
   * @return string (or object if node_id not set) files directory
   */
  public function __construct($node_id = 0) {

    // INIT directoryNames variable
    $this->directoryNames = new stdClass();

    // Get webform nodes info for directory name creation
    $query = Database::getConnection('default', 'default')
      ->select('webform_component', 'wc');
    $query->innerJoin('node', 'n', 'wc.nid = n.nid');
    $query->fields('n', array('nid', 'title', 'uuid'))
      ->condition('n.type', 'webform', 'LIKE');
    // return only single node title
    if ($node_id > 0) {
      $query->condition('n.nid', $node_id);
    }
    $query->groupBy('n.nid'); // Return only one result per nid, since they're all the same across nids.
    $query->orderBy('n.nid');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      $title = substr($result->title, 0, 20);
      $this->directoryNames->{$result->uuid} = $result->nid . '__' . urlencode(str_replace(' ', '-', $title));
    }
    if (count($results) === 1) { // set special property for single output
      $this->singleDir = $this->directoryNames->{$result->uuid};
    }
    else {
      $this->singleDir = "";
    }
  }

  /**
   * Return various file_managed records (based on file_managed.fid) for better directory structure
   * creation for Webform-submitted files during import/rollback.
   *
   * Needs SRC database (eoss-migrate) to work
   *
   * @param string $fid - file ID from either file_managed or file_usage DB tables
   * @return bool if FALSE, else object $result
   */
  public static function getFileInfoFromCustomMap($fid, $data_type = 'nid') {

    // Option 1 - Old FID ==> New NID (and ==> New UUID via extra modifier)
    //// Part 1 - Old FID => Old NID
    if ($data_type == 'nid' || $data_type == 'n_uuid') {
      $data_point = ($data_type == 'nid') ? 'nid' : 'uuid';
      $query = Database::getConnection('default', 'eossMigrate')
        ->select('file_managed', 'fm');
      $query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
      $query->innerJoin('webform_submissions', 'ws', 'fu.id = ws.sid');
      $query->innerJoin('node', 'n', 'ws.nid = n.nid');
      $query->fields('fm', array('fid', 'uri'))
        ->fields('n', array('nid', 'uuid'))
        ->isNotNull('ws.nid')
        ->condition('fm.uri', 'p%', 'LIKE')
        ->condition('fu.module', 'webform', 'LIKE')
        ->condition('fm.fid', $fid)
        ->groupBy('n.nid')
        ->orderBy('ws.nid');
      $results = $query->execute()->fetchAll();
      //// Part 2 - Old NID => New NID
      if (count($results) == 1) {
        $query2 = Database::getConnection('default', 'default')
          ->select('migrate_map_custom_nid', 'mn')
          ->fields('mn', array($data_point))
          ->condition('mn.src_nid', $results[0]->nid);
        $results = $query2->execute()->fetchAll();
      }
    }
    // Option 1 Extra modifier - new NID => New node UUID
    // (ONLY for uuid)
    if ($data_type == 'n_uuid') {
      $query2Addition = Database::getConnection('default', 'default')
        ->select('migrate_map_custom_nid', 'mn')
        ->fields('mn', array('uuid'))
        ->condition('mn.nid', $results[0]->nid);
      $results = $query2Addition->execute()->fetchAll();
    }

    // Option 2 - Old FID => Old URI
    if ($data_type == 'src_uri') {
      $query = Database::getConnection('default', 'eossMigrate')
        ->select('file_managed', 'fm')
        ->fields('fm', array('uri'))
        ->condition('fm.fid', $fid);
      $results = $query->execute()->fetchAll();
    }

    // Option 3 - New FID => Old FID data
    //// Part one: New FID => Old FID
    if ($data_type == 'src_fid') {
      $query = Database::getConnection('default', 'default')
        ->select('migrate_map_migratewebformfilemanagedeoss', 'mfm')
        ->fields('mfm', array('sourceid1'))
        ->condition('mfm.destid1', $fid);
      $results = $query->execute()->fetchAll();
      if (count($results) == 1) {
        //// Part two: Old FID => All Old Fid data
        $query2 = Database::getConnection('default', 'eossMigrate')
          ->select('file_managed', 'fm')
          ->fields('fm')
          ->condition('fm.fid', $results[0]->sourceid1);
        $results = $query2->execute()->fetchAll();
      }
    }

    // Process the results
    if (count($results) <> 1) {
      Migration::displayMessage(__METHOD__ . ' - Number of records for source file ID ' . $fid . ' is not exactly 1.');
      return FALSE;
    }
    else {
      foreach ($results as $result) {
        return $result;
      }
    }
  }

  /**
   * Checks if URI was for a public or private file
   * @param $uri
   * @return string $prefix - Four different permutations of $uri base, based on
   * 1) relative vs. absolute URI, and 2) private vs. public
   *
   */
  public static function isUriPrivate($uri, $migration_side, $absolute = FALSE) {
    if ($absolute) {
      $prefix = ($migration_side == 'dst') ? self::$dstAbsFileFolder : self::$srcAbsFileFolder;
    } else {
      $prefix = '';
    }
    $webform_root = (preg_match('|^private://(.+)$|', $uri))
      ? self::$privateFolder : self::$publicFolder;
    return $prefix . $webform_root;
  }

}
