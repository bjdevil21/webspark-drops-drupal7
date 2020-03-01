<?php

/**
 * Change of Major request form AJAX callback used to update specific AJAX fields listed below.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function asu_rfi_campus_ajax_callback($form, &$form_state) {
  dpm(drupal_add_js(), 'drupal settings from ajax callback');
  $commands = array(
    ajax_command_replace(
      "#asu-rfi-start-date-ajax-wrapper .form-item-start-date",
      render($form['my_interest']['start_date'])),
  );
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}
