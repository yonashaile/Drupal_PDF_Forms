(function ($) {
  app.controller('MainCtrl', ['$scope', '$http', function($scope, $http) {
    $scope.type = $('#PDFFormSigPadFields').attr('data-type');
    $scope.page = $('#PDFFormSigPadFields').attr('data-page');
    $scope.fields = [];
    $scope.activeField = {};
    $scope.activeIndex = null;

    // First get our field data for this page
    getFieldData();

    // Create the fields that retrieved.
    for(var x = 0; x < $scope.fields.length; x++) {
      addSigField($scope.fields[x]);
    }

    /**
    * Get Field data for our page.
    */
    function getFieldData() {
      $http.get('/js/pdf_forms_sigpad/fields/' + $scope.type + '/' + $scope.page).then(
        function(response) {
          $scope.fields = response.data.fields;
        },
        function(error) {
          console.log('error');
          console.log(error);
        }
      );
    }

    /**
    * Add either a new field, or create a field from the database.
    */
    $scope.addSigField = function(field) {
      if (field == null) {
        field = {
          id: null,
          bundle: $scope.type,
          page: $scope.page,
          label: 'Enter Label',
          top: 50,
          left: 50,
          width: 100,
          height: 20,
          value: ''
        };
      }

      $scope.fields.push(field);
      // Allow resizing the fields
    }

    $scope.editSigField = function($index) {
      $('.fieldList li').removeClass('active');
      $("[data-index=" + $index + "]").addClass('active');
      $scope.activeField = $scope.fields[$index];
      $scope.activeIndex = $index;
      $("sig-field[data-index=" + $index + "]")
        .resizable({
          handles: 'all',
          stop: function(event, ui) {
            $scope.fields[$scope.activeIndex].height = $(ui.element).height();
            $scope.fields[$scope.activeIndex].width  = $(ui.element).width();
          }
      });
      $("sig-field[data-index=" + $index + "]")
        .draggable({
          stop: function(event, ui) {
            var position = ui.position;
            $scope.fields[$scope.activeIndex].top = position.top;
            $scope.fields[$scope.activeIndex].left = position.left;
          }
        });
    }

    $scope.saveSettings = function() {
      var req = {
        method: 'post',
        url: '/js/pdf_forms_sigpad/fields/save',
        data: { fields: $scope.fields }
      }
      console.log(req);
      $http.post(req.url, {data : $scope.fields} ).then(
        function(response) {

        },
        function(error) {

        }
      );
    };

  }]);
}(jQuery));
