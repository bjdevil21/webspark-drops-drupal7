<?php

/**
 * Class ThrobberPulse
 */
class ThrobberPulse extends ThrobberBase {

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber sk-spinner sk-spinner-pulse"></div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/pulse.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Pulse';
  }

}