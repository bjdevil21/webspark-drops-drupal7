<?php

/**
 * Class ThrobberTest
 */
class ThrobberTest extends ThrobberBase{

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber"></div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/test.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Test throbber';
  }

}