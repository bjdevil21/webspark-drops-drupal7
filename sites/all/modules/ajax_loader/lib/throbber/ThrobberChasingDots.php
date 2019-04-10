<?php

/**
 * Class ThrobberChasingDots
 */
class ThrobberChasingDots extends ThrobberBase {

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber sk-chasing-dots">
              <div class="sk-child sk-dot1"></div>
              <div class="sk-child sk-dot2"></div>
            </div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/chasing-dots.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Chasing dots';
  }

}