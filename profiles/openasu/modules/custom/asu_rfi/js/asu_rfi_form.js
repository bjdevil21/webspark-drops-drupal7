(function ($) {
  Drupal.behaviors.asu_rfi_form = {
    attach: function (context, settings) {
      // Used to determine if RFI form is on the page.
      // Go to div if it is go to URL if it isn't #block-asu-rfi-asu-rfi-form-block
      $('#take-me-to-rfi').on('click', function (e) {
        e.preventDefault();
        var forms = $('form[id^="asu-rfi-form-data"]');
        if (forms.length === 0) {
          console.log(forms.length);
          console.log(forms.attr('id'));
          location.href = 'https://students.asu.edu/typeofstudent';
        } else {
          console.log(forms.length);
          console.log(forms.attr('id'));
          location.href = '#' + forms.attr('id');
        }
      });
    }
  };
})(jQuery);
