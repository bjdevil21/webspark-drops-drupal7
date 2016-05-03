(function ($) {
  Drupal.behaviors.asu_doc_request = {
    attach: function (context, settings) {


      // Handle show/hide on electronic recipients.

      $('#field-transcript-recipient-coll-values tbody tr:not(:first)').hide();
    
      $( "#edit-commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-e-coll-und-0-field-transcript-copies-p-und-0-value" ).change(function() {
        $('#field-transcript-recipient-coll-values tbody tr').hide();     

        var count = $(this).val();	
	      var i = 0;

	      while (i < count) {
          $('#field-transcript-recipient-coll-values tbody tr:hidden:first').show();
          i++;
        }

        $('#field-transcript-recipient-coll-values tbody tr:hidden input').val("");

      });


      // Keep Shopping Cart Contents quantity and total in sync with form.

      // From the Shopping Cart Contents markup
      var total = $('tbody td.views-field-commerce-order-total');
      var quantity = $('tbody td.views-field-quantity');
      var price = $('tbody td.views-field-commerce-unit-price');
      var unitPrice = price.html().trim().slice(1);

      // If a form change occurs in delivery method radios or one of the # copies inputs.
      $("input[name='commerce_fieldgroup_pane__group_transcript_processing_info[field_transcript_delivery_method][und]'], input#edit-commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-p-coll-und-0-field-transcript-copies-p-und-0-value, input#edit-commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-e-coll-und-0-field-transcript-copies-p-und-0-value").change(function(e){

          // Value selected on form. If a radio, select the appropriate quantity.
          var selectedVal = e.currentTarget.value;
          if (selectedVal == 'Electronic') {
              var updatedQuantity = $('#edit-commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-e-coll-und-0-field-transcript-copies-p-und-0-value').val();
          }
          else if (selectedVal == 'Paper') {
              var updatedQuantity = $('#edit-commerce-fieldgroup-pane-group-transcript-personal-info-field-transcript-info-p-coll-und-0-field-transcript-copies-p-und-0-value').val();
          }
          else { // Input value triggered change, just use it.
              var updatedQuantity = selectedVal;
          }

          // Update the Shopping Cart Contents markup
          quantity.html(updatedQuantity);
          total.html('$' + (unitPrice * updatedQuantity));

      });

    }
  }
}(jQuery));
