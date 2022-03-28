const mqtt = require('mqtt');

const inquirer = require('inquirer');
try {
    var secrets = require("./secrets.json");
} catch (e){
    console.error("You must define a secrets.json file with the broker, username and password!");
    return;
}

var hello_topic = "motorized-cellular-shades/calibrate/hello";


const client = mqtt.connect(secrets.broker, {
    clientId: "shade-boss",
    clean: true,
    connectTimeout: 4000,
    username: secrets.username,
    password: secrets.password,
    reconnectPeriod: 1000,
  });

client.on('connect', function () {
    console.log("Connected to broker, sending HELLO.")
    client.subscribe(hello_topic, function (err) {
        if (err) throw err;

        client.publish(hello_topic, 'hello')

        console.log("Waiting 1 second for responses.")
        setTimeout(pick_shade, 1000)
    });
});

var shades = [];
client.on('message', function (topic, messageBuffer) {
    if (topic == hello_topic) {
        var message = messageBuffer.toString();
        if (message != "hello") {
            console.log("Found shade: " + message);
            shades.push(message);
        }
    }
})



function pick_shade() {
    inquirer
        .prompt([
            {
            type: 'list',
            name: 'shade',
            message: 'Which shade would you like to configure?',
            choices: shades,
            },
        ])
        .then(answers => {
            console.info('Answer:', answers.shade);
            pick_calibration_option(answers.shade);
        });
}

function pick_calibration_option(shade){
    inquirer
    .prompt([
        {
        type: 'list',
        name: 'calibration',
        message: 'What would you like to do with ' + shade + '?',
        choices: ['Set Max Steps', 'Jog', 'Jog Left', 'Jog Right'],
        },
    ])
    .then(answers => {
        console.info('Answer:', answers.calibration);
        if (answers.calibration == 'Set Max Steps') {
            set_max_steps(shade);
        }
    });
}


function set_max_steps(shade) {
    inquirer
    .prompt([
        {
        type: 'integer',
        name: 'max_steps',
        message: 'What is the max steps you would like to set?'
        },
    ])
    .then(answers => {
        console.info('Answer:', answers.max_steps);
        client.publish(shade + "/calibrate/max-steps", answers.max_steps);
        pick_calibration_option(shade);
    });

}
