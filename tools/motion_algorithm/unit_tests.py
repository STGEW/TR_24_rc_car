import logging
import math


from utils import get_wheels_base, get_max_n, get_wheel_radius
import differential_vehicle_model as dvm


logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()]
)


logger = logging.getLogger(__name__)
EPSILON_METERS = 0.05
EPSILON_DEGRESS = 6

config = 'data/config.yml'


wheels_base = get_wheels_base(config)
max_n = get_max_n(config)
wheels_radius = get_wheel_radius(config)


def check_scenario(diff_model, points, init_yaw):
    odo = dvm.OdometerValues()
    pos_diff_dist = dvm.Vehicle2DPosition()
    pos_diff_yaw = dvm.Vehicle2DPosition()
    path = dvm.PathChunk()

    for i in range(len(points)):
        if i == 0:
            logger.info(f"Initial point")
            path.x1 = points[i][0]
            path.y1 = points[i][1]
            path.phi1 = init_yaw
            continue

        path.x2 = points[i][0]
        path.y2 = points[i][1]

        logger.info(
            f"Check point N {i} '{points[i]}'")

        logger.info(
            f"Step 1 - Calculate odo values from distances")

        logger.info(
            f"Path chunk x1: '{path.x1}' y1: '{path.y1}' "
            f"phi1: '{path.phi1}' "
            f"x2: '{path.x2}' y2: '{path.y2}'")

        diff_model.path_to_odometer_values(
            path, odo)
        logger.info(
            f"Odo values angle "
            f"l: '{odo.angle_n_l}' "
            f"r: '{odo.angle_n_r}' "
            f"dist: '{odo.dist_n}' "
            f"phi2: '{odo.phi2}'")


        logger.info(f"Odo values compensate yaw")
        clockwise = odo.angle_n_l > 0
        calc_pos = dvm.Vehicle2DPosition()
        calc_pos.x = 0.0; calc_pos.y = 0.0; calc_pos.phi = 0.0;

        d_yaw_pos = dvm.Vehicle2DPosition()
        d_dist_pos = dvm.Vehicle2DPosition()

        diff_model.odo_to_pos_rotate(
            abs(odo.angle_n_r), clockwise,
            d_yaw_pos)
        logger.info(
            f"Pos rotate x: {d_yaw_pos.x} "
            f"y: {d_yaw_pos.y} "
            f"phi: {d_yaw_pos.phi}")

        forward = True
        diff_model.odo_to_pos_linear(
            abs(odo.dist_n), True,
            path.phi1 + d_yaw_pos.phi,
            d_dist_pos)

        logger.info(
            f"Pos linear x: {d_dist_pos.x} "
            f"y: {d_dist_pos.y} "
            f"phi: {d_dist_pos.phi}")

        calc_pos.x = path.x1 + d_yaw_pos.x + d_dist_pos.x
        calc_pos.y = path.y1 + d_yaw_pos.y + d_dist_pos.y
        calc_pos.phi = path.phi1 + d_yaw_pos.phi + d_dist_pos.phi
    
        logger.info(
            f"Calculated position x: '{calc_pos.x}' "
            f"y: '{calc_pos.y}' "
            f"phi: '{calc_pos.phi}'")


        pos_diff = dvm.Vehicle2DPosition()
        pos_diff.x = path.x2 - calc_pos.x
        pos_diff.y = path.y2 - calc_pos.y
        pos_diff.phi = odo.phi2 - calc_pos.phi

        logger.info(
            f"Difference between estimated and calculated positions "
            f"x: '{pos_diff.x}' "
            f"y: '{pos_diff.y}' "
            f"phi: '{pos_diff.phi}'")

        # error with calculating position by odometer
        # явно ошибка где-то в математике, как я применяю углы и тд
   
        assert pos_diff.x < EPSILON_METERS, f"delta x is bigger than {EPSILON_METERS}"
        assert pos_diff.y < EPSILON_METERS, f"delta y is bigger than {EPSILON_METERS}"
        assert math.degrees(pos_diff.phi) < EPSILON_DEGRESS, f"delta phi is bigger than {EPSILON_DEGRESS}"

        # update x1 position
        path.x1 = path.x2
        path.y1 = path.y2
        path.phi1 = odo.phi2

        logger.info(
            f"Next position point will be x1: '{path.x1}' y1: '{path.y1}' "
            f"phi1: '{path.phi1}'")

def do_scenarios(scenarios):
    for i in range(len(scenarios)):
        s = scenarios[i]
        logger.info(f"Check scenario: {i}. {s}")
        points = s[0]
        init_yaw = s[1]
        diff_model = dvm.DifferentialVehicleModel(
            wheels_base, wheels_radius, max_n)
        check_scenario(diff_model, points, init_yaw)

def test_algo_works_in_both_ways():
    scenarios = [
        [   [[0.0, 0.0],
            [0.5, 0.0],
            [0.5, 0.5],
            [0.0, 0.5],
            [0.0, 0.0]],    # points
            0.0             # yaw
        ],
        [
            [[0.0, 0.0],
            [-0.5, 0.0],
            [-0.5, -0.5],
            [0.0, -0.5],
            [0.0, 0.0]],
            0.0
        ],
        [
            [[0.0, 0.0],
            [0.0, 0.5],
            [0.5, 0.5],
            [0.0, 0.5],
            [0.0, 0.0]],
            0.0
        ],
        [
            [[0.0, 0.0],
            [0.0, -0.5],
            [-0.5, -0.5],
            [0.0, -0.5],
            [0.0, 0.0]],
            0.0
        ],
        [
            [[0.0, 0.0],
            [0.0, -0.5],
            [-0.5, -0.5],
            [0.0, -0.5],
            [0.0, 0.0]],
            2 * 3.14
        ],
        [
            [[0.0, 0.0],
            [0.0, -0.5],
            [-0.5, -0.5],
            [0.0, -0.5],
            [0.0, 0.0]],
            3.14 / 2.0
        ]
        # add different initial yaws
    ]
    do_scenarios(scenarios)


def test_corner_cases():
    scenarios = [
        [
            [[0.0, 0.0],
            [0.0, 0.0]],
            0.0             # yaw
        ],
        [
            [[0.0, 0.0],
            [0.01, 0.0]],
            0.0
        ],
        [
            [[0.0, 0.80]],
            0.0
        ],
        [
            [[0.90, 0.0],
            [0.90, 0.01]],
            0.0
        ],
        [
            [[0.0, 0.90],
            [0.90, 0.01]],
            0.0
        ],
        [
            [[0.0, 0.90],
            [0.0, 0.0]],
            0.0
        ],
        [
            [[0.0, 0.90],
            [0.0, 0.01]],
            0.0
        ],
        [
            [[0.90, 0.90],
            [0.90, 0.90]],
            0.0
        ]
        # add different initial yaws
    ]
    do_scenarios(scenarios)

def main():
    logger.info("test_algo_works_in_both_ways")
    test_algo_works_in_both_ways()

    logger.info("test_corner_cases")
    test_corner_cases()

if __name__ == "__main__":
    main()




