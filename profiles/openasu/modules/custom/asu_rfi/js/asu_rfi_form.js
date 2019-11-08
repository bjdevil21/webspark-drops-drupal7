(function ($) {
  Drupal.behaviors.asu_rfi_form = {
    attach: function (context, settings) {
      // Used to determine if RFI form is on the page.
      // Go to div if it is go to URL if it isn't #block-asu-rfi-asu-rfi-form-block
      $('#take-me-to-rfi').on('click', function (e) {
        e.preventDefault();
        if ($('form[id^="asu-rfi-form-data"]').length === 0) {
          setTimeout(console.log($(this).attr('id')), 5000);
          location.href = 'https://students.asu.edu/typeofstudent';
        } else {
          setTimeout(console.log($(this).attr('id')), 5000);
          location.href = $(this).closest('form').attr('id');
        }
      });
    }
  };
})(jQuery);
