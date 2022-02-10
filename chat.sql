USE chat;

CREATE TABLE `user`
(
id INT AUTO_INCREMENT,
name VARCHAR(50) NOT NULL UNIQUE,
`password` VARCHAR(50) NOT NULL,
state ENUM('online','offline') DEFAULT'offline',
PRIMARY KEY(id)
)ENGINE=INNODB;

CREATE TABLE friend
(
`userid` INT NOT NULL,
`friendid` INT NOT NULL,
PRIMARY KEY(`userid`,`friendid`)
)ENGINE=INNODB;

CREATE TABLE allgroup
(
id INT AUTO_INCREMENT,
groupname VARCHAR(50) NOT NULL UNIQUE,
groupdesc VARCHAR(200) DEFAULT'',
PRIMARY KEY(id)
)ENGINE=INNODB;

CREATE TABLE groupuser
(
groupid INT NOT NULL,
userid INT NOT NULL,
groupprole enum('creator','normal') DEFAULT'normal',
PRIMARY KEY(groupid,userid)
)ENGINE=INNODB;

CREATE TABLE offlinemessage
(
userid INT NOT NULL,
message VARCHAR(500) NOT NULL
)ENGINE=INNODB;

