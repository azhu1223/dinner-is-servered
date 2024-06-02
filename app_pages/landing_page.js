document.getElementById('gram-form').addEventListener('submit', function(event) {
    event.preventDefault(); // Prevent the default form submission behavior

    // Show loading message and disable submit button
    document.getElementById('loading').style.display = 'block';
    const submitButton = document.getElementById('submit-button');
    submitButton.disabled = true;

    // Create a FormData object
    const formData = new FormData();

    // Get the files and append them to the FormData object
    const files = document.getElementById('file-upload').files;
    for (let i = 0; i < files.length; i++) {
        formData.append('file-upload', files[i]);
    }

    // Get the relevant information and append it to the FormData object
    const relevantInfo = document.getElementById('relevant-info').value;
    formData.append('relevant-info', relevantInfo);

    // Send a POST request with the form data
    fetch('/app', {
        method: 'POST',
        body: formData
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }
        return response.text(); // Convert the response to plain text
    })
    .then(text => {
        console.log('Success:', text);

        // Hide loading message
        document.getElementById('loading').style.display = 'none';

        // Display response
        const responseDiv = document.getElementById('response');
        responseDiv.style.display = 'block';
        responseDiv.innerHTML = `
            <h2>Response Received</h2>
            <pre>${text}</pre>
        `;

        // Enable the submit button
        submitButton.disabled = false;
    })
    .catch((error) => {
        console.error('Error:', error);

        // Hide loading message
        document.getElementById('loading').style.display = 'none';

        // Display error message
        const responseDiv = document.getElementById('response');
        responseDiv.style.display = 'block';
        responseDiv.innerHTML = `
            <h2>Error</h2>
            <p>There was an error submitting the form. Please try again.</p>
        `;

        // Enable the submit button
        submitButton.disabled = false;
    });

});
