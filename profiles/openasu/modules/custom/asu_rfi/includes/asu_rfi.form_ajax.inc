<?php

/**
 * Change of Major request form AJAX callback used to update specific AJAX fields listed below.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function _asu_rfi_campus_ajax_callback($form, &$form_state) {
  $commands = array(
    ajax_command_replace("#asu-rfi-start-date-ajax-wrapper", render($form['my_interests']['start_date'])),
  );
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}
