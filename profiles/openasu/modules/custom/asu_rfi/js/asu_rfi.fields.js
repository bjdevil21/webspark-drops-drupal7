(function ($) {
  Drupal.behaviors.asuRfiFields = {
    attach: function (context, settings) {
      // Give GA time to load.
      setTimeout(function () {
        // Set the EdPlus Lead API enterpriseclientid if possible.
        if ('undefined' != typeof ga) {
          ga(function () {
            //var extraFields = "";
            var gaIds = ga.getAll();
            console.log(gaIds);
            var i, size, match;
            for (i = 0, size = gaIds.length, match = 0; i < size; i++) {
              console.log(gaIds[i]);
              if (gaIds[i].get('trackingId') === 'UA-42798992-4') {
                //The field name sent to the Lead API should be enterpriseclientid
                //You can use an existing field or create it dynamically
                alert(gaIds[1]);
                $('input[name="enterpriseclientid"]').val(gaIds[i].get('clientId'));
                // This counter should be equal to the number of GA accounts to be read
                if (++match === 1)
                  break;
              }
            }
          });
        }
      }, 5000);
    }
  }
})(jQuery);
