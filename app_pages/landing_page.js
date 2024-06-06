document.getElementById('gram-form').addEventListener('submit', function(event) {

    //Hide the response message if it is there from previous requests
    const responseDiv = document.getElementById('response');
    if (responseDiv) {
        responseDiv.style.display = 'none';
    }

    event.preventDefault(); // Prevent the default form submission behavior

    // Show loading message and disable submit button
    document.getElementById('loading').style.display = 'block';
    const submitButton = document.getElementById('submit-button');
    submitButton.disabled = true;

    // Create a FormData object
    const formData = new FormData();

    // Get the files and convert them to base64 encoded strings
    const files = document.getElementById('file-upload').files;
    const fileReaders = [];
    const base64Files = [];

    for (let i = 0; i < files.length; i++) {
        const reader = new FileReader();
        fileReaders.push(new Promise((resolve, reject) => {
            reader.onload = function(event) {
                base64Files.push(event.target.result);
                resolve();
            };
            reader.onerror = function(error) {
                reject(error);
            };
            reader.readAsDataURL(files[i]);
        }));
    }

    // Wait for all files to be read and then append them to the FormData object
    Promise.all(fileReaders).then(() => {
        for (let i = 0; i < base64Files.length; i++) {
            formData.append('file-upload', base64Files[i]);
        }

        // Append the number of images
        formData.append('number-of-images', files.length);

        // Get the relevant information and append it to the FormData object
        const relevantInfo = document.getElementById('relevant-info').value;
        formData.append('relevant-info', relevantInfo);

        // Send a POST request with the form data
        return fetch('/app', {
            method: 'POST',
            body: formData
        });
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }
        return response.json(); // Convert the response to json
    })
    .then(json => {
        console.log('Success:', json);

        // Hide loading message
        document.getElementById('loading').style.display = 'none';

        // Display response
        const reader = new FileReader();
        const image_file = files[json.best_image_index];
        reader.onload = function(e) {
            responseDiv.style.display = 'block';
            responseDiv.innerHTML = `
                <h2>Best Instagram Post</h2>
                <img id="image-display" src="${e.target.result}" alt="Best Instagram Photo" style="max-width:100%;">
                <pre style="text-wrap: wrap;">${json.caption}</pre>
            `;
        }
        reader.readAsDataURL(image_file);

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
