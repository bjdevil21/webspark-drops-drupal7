(function ($) {
  Drupal.behaviors.asu_doc_request = {
    attach: function (context, settings) {



      // Keep Shopping Cart Contents quantity and total in sync with form.

      // From the Shopping Cart Contents markup
      var total = $('tbody td.views-field-commerce-order-total');
      var quantity = $('tbody td.views-field-quantity');
      var price = $('tbody td.views-field-commerce-unit-price');
      var unitPrice = price.html().trim().slice(1);

      // Handle show/hide on electronic recipients...

      $('#field-transcript-recipient-coll-values tbody tr:not(:first)').hide();
      
        // ...after clicking 'up' or 'down' arrow
      $("#commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-e-coll-und-0-field-transcript-copies-p-add-more-wrapper a.spinint-up, #commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-e-coll-und-0-field-transcript-copies-p-add-more-wrapper a.spinint-down").click(function() {
        
        $('#field-transcript-recipient-coll-values tbody tr').hide();

        var count = $('#commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-e-coll-und-0-field-transcript-copies-p-add-more-wrapper .spinint-scroller span.spinint').text();
	      var i = 0;

	      while (i < count) {
          $('#field-transcript-recipient-coll-values tbody tr:hidden:first').show();
          i++;
        }

        $('#field-transcript-recipient-coll-values tbody tr:hidden input').val("");

        
        quantity.html(count);
        total.html('$' + (unitPrice * count));

      });

      $("#commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-p-coll-und-0-field-transcript-copies-p-add-more-wrapper a.spinint-up, #commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-p-coll-und-0-field-transcript-copies-p-add-more-wrapper a.spinint-down").click(function() {
        
        var count = $('#commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-p-coll-und-0-field-transcript-copies-p-add-more-wrapper .spinint-scroller span.spinint').text();

        quantity.html(count);
        total.html('$' + (unitPrice * count));

      });

    }
  }
}(jQuery));
