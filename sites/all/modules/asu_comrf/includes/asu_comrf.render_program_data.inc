<?php

/**
 * Change of Major request form AJAX callback used to update specific AJAX fields listed below.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function asu_comrf_new_major_choice_ajax_callback($form, &$form_state) {
  $commands = array(
    ajax_command_replace("#asu-comrf-new-campus-choice-ajax-wrapper", render($form['asu_comrf_new_campus_choice'])),
    ajax_command_replace("#asu-comrf-com-requirements-text-ajax-wrapper", render($form['asu_comrf_com_requirements_text'])),
    ajax_command_replace("#asu-comrf-requirements-check-ajax-wrapper", render($form['asu_comrf_requirements_check'])),
    ajax_command_replace("#asu-comrf-double-check-ajax-wrapper", render($form['asu_comrf_double_check'])),
    ajax_command_replace("#asu-comrf-contact-phone-number-ajax-wrapper", render($form['asu_comrf_contact_phone_number'])),
    ajax_command_replace("#asu-comrf-major-change-source-ajax-wrapper", render($form['asu_comrf_major_change_source'])),
    ajax_command_replace("#asu-comrf-major-change-source-other-ajax-wrapper", render($form['asu_comrf_major_change_source_other'])),
    ajax_command_replace("#asu-comrf-why-change-ajax-wrapper", render($form['asu_comrf_why_change'])),
    ajax_command_replace("#asu-comrf-why-change-other-ajax-wrapper", render($form['asu_comrf_why_change_other'])),
    ajax_command_replace("#asu-comrf-what-if-audit-text-ajax-wrapper", render($form['asu_comrf_what_if_audit_text'])),
    ajax_command_replace("#asu-comrf-form-footer-text-ajax-wrapper", render($form['asu_comrf_form_footer_text'])),
    ajax_command_replace("#asu-comrf-submit-ajax-wrapper", render($form['asu_comrf_submit'])),
    ajax_command_replace("#asu-comrf-wpc-com-request-submit-ajax-wrapper", render($form['asu_comrf_wpc_com_request_submit'])),
    ajax_command_replace("#asu-comrf-new-subplan-choice-ajax-wrapper", render($form['asu_comrf_new_subplan_choice'])),
  );
  // which CSS logic here or module file?
  $css_display = ($form['asu_comrf_new_subplan_choice']['#required'] == TRUE) ? 'block' : 'none';
  $commands[] = ajax_command_css("#asu-comrf-new-subplan-choice-ajax-wrapper", array('display' => $css_display));
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}
