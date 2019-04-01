<?php

/**
 * Creates directory information for file moves/migrations
 */
class MigrateWebformFileDirectories {

  /**
   * Directories for file migration (src, dest)
   * @var
   */
  protected static $srcFolder = '/home/bryan/eoss-migrate/sites/default/files/webform/';
  protected static $dstFolder = DRUPAL_ROOT . '/sites/default/files/webform/';
  public static function getSrcFolder() {
    return self::$srcFolder;
  }
  public static function getDstFolder() {
    return self::$dstFolder;
  }

  /**
   * @var (obj upon instantiation) List of directory names
   */
  protected $directoryNames;
  public function getDirectoryNames() {
    return $this->directoryNames;
  }

  /**
   * MigrateWebformFileDirectories constructor.
   *
   * Builds default directory names list upon each instantiation.
   * @param $node_id -
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
   * Return information about file in file_managed.fid for better directory structure
   * for webform submitted files.
   *
   * @param string $fid - file ID from either file_managed or file_usage DB tables
   * @return bool if FALSE, else object $result
   */
  public static function getFileInfoFromCustomMap($fid) {

    // @TODO add uuid column to fu and fill it up on eoss-migrate
    $query = Database::getConnection('default', 'default')
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

    var_dump(__METHOD__);

    $results = $query->execute()->fetchAll();

    var_dump("----------------- fid");
    var_dump($fid);
    var_dump("----------------- results");
    var_dump($results);
    var_dump("-----------------");

    if (count($results) <> 1) {
      Migration::displayMessage(__METHOD__ . ' - Number of records for file ID ' . $fid . ' is not exactly 1.');
      return FALSE;
    }
    else {
      foreach ($results as $result) {
        return $result;
      }
    }
  }

}
