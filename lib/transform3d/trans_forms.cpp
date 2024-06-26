/*
 * @Descripttion: cpp实现
 * @Author: sangxin
 * @Date: 2021-05-01 21:04:19
 * @LastEditTime: 2021-05-02 23:39:20
 */
#include "transforms3d.h"

/*---------------------------------------角度弧度转换----------------------------------------*/
/**
 * @brief: 角度转为弧度
 * @param {double} angle 角度值
 * @return 返回对应弧度值,一般在-3.14~3.14之间
 */
#if VERTICES_CONFIG_CXX_STANDARD >= 20
double TransForms::Degrees(double angle) { return angle / 180 * std::numbers::pi; }
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
double TransForms::Degrees(double angle) { return angle / 180 * M_PI; }
#endif

/**
 * @brief: 弧度转为角度
 * @param {double} degrees 弧度值
 * @return 返回对应的角度值，一般在-180~180之间
 */
#if VERTICES_CONFIG_CXX_STANDARD >= 20
double TransForms::Angle(double degrees) { return degrees / std::numbers::pi * 180; }
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
double TransForms::Angle(double degrees) { return degrees / M_PI * 180; }
#endif

/*---------------------------------------欧拉角部分---------------------------*/
/**
 * @brief: 角度制欧拉角转旋转矩阵,此函数默认的旋转顺序是x-y-z.
 * @param {double} rx 绕x轴的旋转.
 * @param {double} ry 绕y轴的旋转.
 * @param {double} rz 绕z轴的旋转.
 * @return {Matrix3d}  返回3✖3的旋转矩阵.
 */
Eigen::Matrix3d TransForms::EulerAngle2Mat(double rx, double ry, double rz)
{
  rx = Degrees(rx);
  ry = Degrees(ry);
  rz = Degrees(rz);
  Eigen::AngleAxisd rollAngle(Eigen::AngleAxisd(rx, Eigen::Vector3d::UnitX()));
  Eigen::AngleAxisd pitchAngle(Eigen::AngleAxisd(ry, Eigen::Vector3d::UnitY()));
  Eigen::AngleAxisd yawAngle(Eigen::AngleAxisd(rz, Eigen::Vector3d::UnitZ()));
  Eigen::Matrix3d rotation_matrix;
  rotation_matrix = yawAngle * pitchAngle * rollAngle;
  return rotation_matrix;
}
/**
 * @brief: 欧拉角转旋转矩阵
 * @param {Vector3d} eular 欧拉角rx,ry,rz
 * @return {Matrix3d} 返回3✖3的旋转矩阵.
 */
Eigen::Matrix3d TransForms::Euler2Mat(double rx, double ry, double rz)
{
  Eigen::AngleAxisd rollAngle(Eigen::AngleAxisd(rx, Eigen::Vector3d::UnitX()));
  Eigen::AngleAxisd pitchAngle(Eigen::AngleAxisd(ry, Eigen::Vector3d::UnitY()));
  Eigen::AngleAxisd yawAngle(Eigen::AngleAxisd(rz, Eigen::Vector3d::UnitZ()));
  Eigen::Matrix3d rotation_matrix;
  rotation_matrix = yawAngle * pitchAngle * rollAngle;
  return rotation_matrix;
}

/**
 * @brief:欧拉角转四元数
 * @param {double} rx 绕x轴的旋转
 * @param {double} ry 绕y轴的旋转
 * @param {double} rz 绕z轴的旋转
 * @return {Quaterniond} 返回对应的四元数
 */
Eigen::Quaterniond TransForms::Euler2Quat(double rx, double ry, double rz)
{
  return Eigen::AngleAxisd(rx, ::Eigen::Vector3d::UnitX()) *
         Eigen::AngleAxisd(ry, ::Eigen::Vector3d::UnitY()) *
         Eigen::AngleAxisd(rz, ::Eigen::Vector3d::UnitZ());
}

/**
 * @brief: 角度制欧拉角转四元数
 * @param {double} rx 绕x轴的旋转
 * @param {double} ry 绕y轴的旋转
 * @param {double} rz 绕z轴的旋转
 * @return {Quaterniond} 返回对应的四元数
 */
Eigen::Quaterniond TransForms::EulerAngle2Quat(double rx, double ry, double rz)
{
  rx = Degrees(rx);
  ry = Degrees(ry);
  rz = Degrees(rz);
  return Eigen::AngleAxisd(rx, ::Eigen::Vector3d::UnitX()) *
         Eigen::AngleAxisd(ry, ::Eigen::Vector3d::UnitY()) *
         Eigen::AngleAxisd(rz, ::Eigen::Vector3d::UnitZ());
}
/**
 * @brief: 旋转矩阵转欧拉角(弧度制)
 * @param {Matrix3d} 3✖3的旋转矩阵
 * @return {Vector3d} 欧拉角
 */
Eigen::Vector3d TransForms::Mat2Euler(Eigen::Matrix3d mat)
{
  return mat.eulerAngles(0, 1, 2);
}

/**
 * @brief: 欧拉角转旋转矩阵
 * @param {double} rx 绕x轴的旋转
 * @param {double} ry 绕y轴的旋转
 * @param {double} rz 绕z轴的旋转
 * @return {*}
 */
Eigen::Matrix3d TransForms::EulerAngle2Mat(Eigen::Vector3d eular)
{
  return EulerAngle2Mat(eular.x(), eular.y(), eular.z());
}

/**
 * @brief: 旋转矩阵转角度制欧拉角(角度制)
 * @param {Matrix3d} 3✖3的旋转矩阵
 * @return {Vector3d}  欧拉角
 */
Eigen::Vector3d TransForms::Mat2EulerAngle(Eigen::Matrix3d mat)
{
  Eigen::Vector3d rot = mat.eulerAngles(2, 1, 0);
#if VERTICES_CONFIG_CXX_STANDARD >= 20
    rot = rot / std::numbers::pi * 180;
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
  rot = rot / M_PI * 180;
#endif
  return rot;
}

/*---------------------------------------四元数部分----------------------------------------*/
/**
 * @brief: 四元数转旋转矩阵
 * @param {Quaterniond} 四元数
 * @return {Matrix3d} 对应的旋转矩阵
 */
Eigen::Matrix3d TransForms::Quat2Mat(Eigen::Quaterniond quat) { return quat.matrix(); }

/**
 * @brief: 四元数转欧拉角
 * @param {Quaterniond} 四元数
 * @return {Vector3d} 对应的欧拉角
 */
Eigen::Vector3d TransForms::Quat2Eular(Eigen::Quaterniond quat)
{
  return Mat2Euler(quat.matrix());
}

/**
 * @brief: 四元数转弧度制欧拉角(角度制)
 * @param {Quaterniond} 四元数
 * @return {Vector3d} 对应的欧拉角
 */
Eigen::Vector3d TransForms::Quat2EularAngle(Eigen::Quaterniond quat)
{
  return Mat2EulerAngle(quat.matrix());
}

/**
 * @brief: 旋转矩阵转四元数
 * @param {Matrix3d} 3✖3的旋转矩阵
 * @return {Quaterniond} 对应的四元数
 */
Eigen::Quaterniond TransForms::Mat2Quat(Eigen::Matrix3d mat) { return Eigen::Quaterniond(mat); }

/*---------------------------------------齐次矩阵部分----------------------------------------*/
/**
 * @brief: 通过位置和欧拉角合成一个齐次矩阵
 * @param {Vector3d} positon 平移位置
 * @param {Vector3d} rotEular  旋转变换(欧拉角形式)
 * @return {*}
 */
Eigen::Matrix4d TransForms::Compose(Eigen::Vector3d positon, Eigen::Vector3d rotEular)
{
  Eigen::Matrix3d rot = TransForms::EulerAngle2Mat(rotEular);
  // std::cout<<Mat2EulerAngle(rot);
  Eigen::Matrix4d t;
  t.setIdentity();
  t.block<3, 3>(0, 0) = rot;
  t.block<3, 1>(0, 3) = positon;
  return t;
}

/**
 * @brief: 通过位置和四元数合成一个齐次矩阵
 * @param {Vector3d} positon 平移位置
 * @param {Quaterniond} quat 四元数
 * @return {Matrix4d} 齐次矩阵
 */
Eigen::Matrix4d TransForms::Compose(Eigen::Vector3d positon, Eigen::Quaterniond quat)
{
  return Compose(positon, Quat2Eular(quat));
}

/**
 * @brief: 通过三个位置和三个欧拉角合成一个齐次矩阵
 * @param {double} x 沿x轴的平移
 * @param {double} y 沿y轴的平移
 * @param {double} z 沿z轴的平移
 * @param {double} rx 绕x轴的旋转
 * @param {double} ry 绕y轴的旋转
 * @param {double} rz 绕z轴的旋转
 * @return {Matrix4d} 返回4✖4的齐次变换矩阵
 */
Eigen::Matrix4d TransForms::ComposeEuler(const double x, const double y,
                                  const double z, const double rx,
                                  const double ry, const double rz)
{
  Eigen::Vector3d rot(rx, ry, rz);
  Eigen::Vector3d pos(x, y, z);
  return TransForms::Compose(pos, rot);
}

/**
 * @brief:  将齐次矩阵转换成平移和欧拉角形式，方便理解
 * @param {Matrix4d} 4✖4的齐次变换矩阵
 * @return {VectorXd} x,y,z,rx,ry,rz
 */
Eigen::VectorXd TransForms::H2EulerAngle(Eigen::Matrix4d t)
{
  Eigen::VectorXd pose = Eigen::VectorXd(6);
  Eigen::Matrix3d mt = t.block<3, 3>(0, 0);
  Eigen::Vector3d p3 = t.block<3, 1>(0, 3).col(0);
  pose(0, 0) = p3.x();
  pose(1, 0) = p3.y();
  pose(2, 0) = p3.z();
  Eigen::Vector3d eular = Mat2EulerAngle(mt);
  pose(3, 0) = eular.x();
  pose(4, 0) = eular.y();
  pose(5, 0) = eular.z();
  return pose;
}

/**
 * @brief:  将齐次矩阵转换成平移和旋转矩阵形式
 * @param {Matrix4d} 4✖4的齐次变换矩阵
 * @return {VectorXd} x,y,z,rx,ry,rz
 */
Eigen::Matrix3d TransForms::HDecompose(Eigen::Matrix4d t, Eigen::Matrix3d &rotate,
                                Eigen::Vector3d &position)
{
  rotate = t.block<3, 3>(0, 0);
  position = t.block<3, 1>(0, 3).col(0);
  return rotate;
}