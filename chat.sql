USE chat;

drop table if exists `user`;
CREATE TABLE `user`
(
id INT AUTO_INCREMENT,
name VARCHAR(50) NOT NULL UNIQUE,
`password` VARCHAR(50) NOT NULL,
state ENUM('online','offline') DEFAULT'offline',
PRIMARY KEY(id)
)ENGINE=INNODB;

drop table if exists `friend`;
CREATE TABLE friend
(
`userid` INT NOT NULL,
`friendid` INT NOT NULL,
PRIMARY KEY(`userid`,`friendid`)
)ENGINE=INNODB;

drop table if exists `allgroup`;
CREATE TABLE allgroup
(
id INT AUTO_INCREMENT,
groupname VARCHAR(50) NOT NULL UNIQUE,
groupdesc VARCHAR(200) DEFAULT'',
PRIMARY KEY(id)
)ENGINE=INNODB;

drop table if exists `groupuser`;
CREATE TABLE groupuser
(
groupid INT NOT NULL,
userid INT NOT NULL,
grouprole enum('creator','normal') DEFAULT'normal',
PRIMARY KEY(groupid,userid)
)ENGINE=INNODB;

drop table if exists `offlinemessage`;
CREATE TABLE offlinemessage
(
userid INT NOT NULL,
message VARCHAR(500) NOT NULL
)ENGINE=INNODB;
