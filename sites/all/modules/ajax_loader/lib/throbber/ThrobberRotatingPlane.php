<?php

/**
 * Class ThrobberRotatingPlane
 */
class ThrobberRotatingPlane extends ThrobberBase {

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber sk-rotating-plane"></div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/rotating-plane.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Rotating plane';
  }

}