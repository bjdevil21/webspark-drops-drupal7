(function ($) {
  Drupal.behaviors.asu_rfi_form = {
    attach: function (context, settings) {
      // Used to determine if RFI form is on the page.
      // Go to div if it is go to URL if it isn't #block-asu-rfi-asu-rfi-form-block
      $('#take-me-to-rfi').on('click', function (e) {
        e.preventDefault();
        console.log($('form[id^="asu-rfi-form-data"]').length);
        console.log($(this).length);
        console.log(this.length);
        console.log('this');
        console.log(this.length);
        console.log(this.closest("form").attr('id'));
        console.log('this 2');
        console.log($(this).closest("form").attr('id'));
/*        location.href = ($(this).length === 0)
          ? 'https://students.asu.edu/typeofstudent'
          : $(this).closest("form").attr('id'); */
      });
    }
  };
})(jQuery);
