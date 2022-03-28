const mqtt = require('mqtt');

const inquirer = require('inquirer');
try {
    var secrets = require("./secrets.json");
} catch (e){
    console.error("You must define a secrets.json file with the broker, username and password!");
    return;
}

var greeting_topic = "motorized-cellular-shades/greetings";


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
    how_to_find_shades();
});

function how_to_find_shades() {
    inquirer
        .prompt([
            {
            type: 'list',
            name: 'message',
            message: 'How would you like to find the shade?',
            choices: ['hello', 'announce'],
            },
        ])
        .then(answers => {
            console.info('Answer:', answers.message);
            var msg = answers.message == 'hello' ? '__hello__' : '__announce__';
            find_shades(msg);
        });
}

var shades = [];
client.on('message', function (topic, messageBuffer) {
    if (topic == greeting_topic) {
        var message = messageBuffer.toString();
        if (message != "__hello__" && message != "__announce__") {
            console.log("Found shade: " + message);
            shades.push(message);
        }
    }
})

function find_shades(msg) {
    client.subscribe(greeting_topic, function (err) {
        if (err) throw err;

        client.publish(greeting_topic, msg)

        console.log("Waiting 1 second for responses.")
        setTimeout(check_for_shades, 1000)
    });
}

function check_for_shades() {
    if (shades.length == 0) {
        console.log("No shades found.");
        how_to_find_shades();
    } else {
        pick_shade();
    }
}

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
        choices: ['Set Max Steps', 'Set Position', 'Jog', 'Jog Left', 'Jog Right'],
        },
    ])
    .then(answers => {
        console.info('Answer:', answers.calibration);
        if (answers.calibration == 'Set Max Steps') {
            set_max_steps(shade);
        } else if (answers.calibration == 'Set Position') {
            set_position(shade);
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

function set_position(shade) {
    inquirer
    .prompt([
        {
        type: 'integer',
        name: 'position',
        message: 'What is the position you would like to set?'
        },
    ])
    .then(answers => {
        console.info('Answer:', answers.position);
        client.publish(shade + "/calibrate/position", answers.position);
        pick_calibration_option(shade);
    });

}
