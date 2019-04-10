<?php

/**
 * Class ThrobberDoubleBounce
 */
class ThrobberDoubleBounce extends ThrobberBase {

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber sk-double-bounce">
                <div class="sk-child sk-double-bounce1"></div>
                <div class="sk-child sk-double-bounce2"></div>
            </div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/double-bounce.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Double bounce';
  }

}